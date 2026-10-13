#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <set>
#include <string>
#include <tuple>
#include <vector>

#include "zelda_config.h"
#include "zelda_render.h"
#include "librecomp/helpers.hpp"
#include "recomp.h"

// Widescreen support for the game's 2D layer.
//
// The game draws full-screen fades, menu backdrops and dimming overlays as
// rectangles spanning the 320-pixel framebuffer, which RT64 keeps inside the
// 4:3 area when the aspect ratio is expanded. Its extended GBI can instead pin
// a rectangle's edges to the real screen edges, but the commands are wider
// than the originals, so they can't be patched in place. This walks the
// display list just before it is submitted (hook in nnScExecuteGraphics) and
// replaces each full-width rectangle with a gSPDisplayList branch, which is
// the same size, into a small list of ours holding the edge-aligned version.
//
// The game uses F3DEX 1.23, so the opcodes below are that microcode's.
namespace {
    constexpr uint8_t op_spnoop = 0x00;
    constexpr uint8_t op_dl = 0x06;
    constexpr uint8_t op_rdphalf_2 = 0xB3;
    constexpr uint8_t op_rdphalf_1 = 0xB4;
    constexpr uint8_t op_enddl = 0xB8;
    constexpr uint8_t op_texrect = 0xE4;
    constexpr uint8_t op_texrectflip = 0xE5;
    constexpr uint8_t op_fillrect = 0xF6;
    constexpr uint8_t op_settilesize = 0xF2;
    constexpr uint8_t op_settile = 0xF5;
    constexpr uint8_t op_mtx = 0x01;
    constexpr uint8_t op_vtx = 0x04;
    constexpr uint8_t op_moveword = 0xBC;
    constexpr uint8_t mw_segment = 0x06;

    // RSP segment table as set by gSPSegment while walking; the game keeps
    // its 2D display lists behind segment 1.
    uint32_t segments[16] = {};

    // Resolve a display list or vertex address to a physical RDRAM address.
    uint32_t resolve(uint32_t addr) {
        if ((addr & 0xF0000000u) == 0x80000000u || (addr & 0xF0000000u) == 0xA0000000u) {
            return addr & 0x1FFFFFFFu;
        }
        uint32_t segment = (addr >> 24) & 0xF;
        return (segments[segment] & 0x1FFFFFFFu) + (addr & 0x00FFFFFFu);
    }

    // Texture state per tile descriptor as the walk sees it, so a texture
    // rectangle can be judged by what it draws: a full-width strip of a
    // 320-wide image must stay centred, a strip of a small repeating tile is
    // a backdrop that should reach the screen edges.
    struct TileState {
        int width = 0;
        int height = 0;
        bool clamp_s = false;
    };
    TileState tiles[8];
    // Textures at most this wide are treated as repeating backdrops.
    constexpr int backdrop_max_texture_width = 128;

    // HUD layout (Enhancements > HUD). The HP/MP block (top-left, y 24-60)
    // and the four element gems with their level digits (bottom-left,
    // y 197-213) are texture rectangles drawn from x = 0 in the 320-wide
    // frame, which RT64 keeps inside the centred 4:3 area. When a block has
    // a custom position, each of its rectangles is rebuilt with both edges
    // on the LEFT origin (x counted from the real left edge of the window,
    // in the same frame pixels) at the block's position plus the
    // rectangle's offset within the block. Rectangles are recognised by
    // zone and by the textures the HUD is made of (from the frame dumps):
    // the digit strip 129x11, the HP/MP labels 63x18 and 63x19, the bar
    // 57x6 and the 13x13 gems.
    struct HudBlock {
        std::atomic<bool> custom = false;
        std::atomic<float> x = 0.0f;
        std::atomic<float> y = 0.0f;
    };
    HudBlock hud_hp;
    HudBlock hud_spirits;
    constexpr int hud_max_x = 160;   // the fourth gem's digits reach x 107; three-digit HP goes wider
    constexpr int hud_hp_anchor_y = 24;
    constexpr int hud_sp_anchor_y = 197;

    // Which block a texture rectangle belongs to, or null.
    HudBlock* hud_block_of(int ulx, int uly, const TileState& tile) {
        if (ulx >= hud_max_x) {
            return nullptr;
        }
        // The bar's texture is loaded only as wide as the HP or MP left
        // (57x6 at full), and the plates likewise while the block slides
        // in, so a HUD texture is known by its height and at most its full
        // width.
        bool hud_texture = (tile.width <= 129 && tile.height == 11) || (tile.width <= 63 && (tile.height == 18 || tile.height == 19))
            || (tile.width <= 57 && tile.height == 6) || (tile.width == 13 && tile.height == 13);
        if (!hud_texture) {
            return nullptr;
        }
        if (uly >= 16 && uly <= 72) {
            return &hud_hp;
        }
        // Hard Mode lays the gems out as a triangle (iconPositionsTriangle
        // in its source): the fire gem sits 22 rows above the vanilla row,
        // at y 175, so the zone starts well above it.
        if (uly >= 168 && uly <= 224) {
            return &hud_spirits;
        }
        return nullptr;
    }

    constexpr uint32_t ex_opcode = 0x64;
    constexpr uint32_t ex_fillrect = 0x000003;
    constexpr uint32_t ex_texrect = 0x000002;
    constexpr uint32_t ex_origin_left = 0x0;
    constexpr uint32_t ex_origin_right = 0x400;
    constexpr uint32_t ex_origin_none = 0x800;
    // gEXEnable for a non-F3DEX2 microcode: the hook opcode is G_SPNOOP.
    constexpr uint32_t ex_enable_w0 = 0x00525464;
    constexpr uint32_t ex_enable_w1 = (0x1u << 28) | ex_opcode;

    // The game never calls osGetMemSize and assumes 4MB, so the top of the
    // 8MB the runtime provides is free. Sub-lists are written into this ring
    // and are only a few commands each.
    constexpr int32_t sublist_ring_start = 0x807E0000;
    constexpr int32_t sublist_ring_end = 0x80800000;
    int32_t sublist_cursor = sublist_ring_start;

    constexpr int screen_width = 320;
    // A rectangle that reaches (near) both sides of the 4:3 area. The frame
    // clear uses an 8-pixel border, so allow that much.
    constexpr int full_width_margin = 8;

    constexpr int max_depth = 16;
    constexpr int max_commands = 1 << 16;

    std::atomic<bool> enabled = true;
    // Frame capture, for chasing frame interpolation glitches (a draw whose
    // matrix or vertex count changes between two frames is what RT64
    // mismatches). Only when frame_capture.txt is in the app folder: every
    // frame's whole command stream, with vertices and decoded matrices, is
    // kept as text in a ring of the last capture_frames frames, and the F9
    // key writes the ring out, oldest first, to frame_capture/. A glitch
    // lasts a few frames and is over before a key can be pressed, so the
    // record has to be of what already happened.
    constexpr int capture_frames = 60;
    std::vector<std::string> capture_ring(capture_frames);
    int capture_next = 0;
    int capture_count = 0;
    int capture_enabled = -1; // -1 = not checked yet
    std::atomic<bool> capture_save_requested = false;
    std::string* dump_buf = nullptr;

    void dump(const char* fmt, auto... args) {
        if (dump_buf) {
            char line[256];
            int n = snprintf(line, sizeof(line), fmt, args...);
            if (n > 0) {
                dump_buf->append(line, std::min(n, static_cast<int>(sizeof(line)) - 1));
            }
        }
    }

    void save_capture() {
        std::filesystem::path dir = zelda64::get_app_folder_path() / "frame_capture";
        std::error_code ec;
        std::filesystem::remove_all(dir, ec);
        std::filesystem::create_directories(dir, ec);
        int first = (capture_next - capture_count + capture_frames) % capture_frames;
        for (int i = 0; i < capture_count; i++) {
            char name[32];
            snprintf(name, sizeof(name), "frame_%02d.txt", i);
            FILE* f = fopen((dir / name).string().c_str(), "wb");
            if (f) {
                const std::string& text = capture_ring[(first + i) % capture_frames];
                fwrite(text.data(), 1, text.size(), f);
                fclose(f);
            }
        }
    }

    // Diagnostics: every distinct rectangle seen, written once.
    std::mutex log_mutex;
    std::set<std::tuple<int, int, int, int, int>> logged_rects;
    // Calibration aid: off while playing. It had grown a 14 MB file by the
    // time anyone noticed.
    bool log_rects = false;

    // Whether [addr, addr + bytes) lies in the 8MB the game can address. A
    // display list overrun used to write commands over the other frame
    // block's task, so its list pointer could be anything; the RDP masks
    // what it is given, but reading outside the recomp's RDRAM faults, and
    // that was a crash (1.7.1, a busy Real Time Combat battle). Nothing
    // outside this range is walked.
    bool in_rdram(int32_t addr, uint32_t bytes) {
        uint32_t a = static_cast<uint32_t>(addr);
        return a >= 0x80000000u && a <= 0x80800000u - bytes;
    }

    uint32_t read_w(uint8_t* rdram, int32_t addr) {
        return static_cast<uint32_t>(MEM_W(0, addr));
    }

    void write_w(uint8_t* rdram, int32_t addr, uint32_t value) {
        MEM_W(0, addr) = static_cast<int32_t>(value);
    }

    // Whether the field HUD can be on screen. The pause screen (gGameMode 2)
    // and the Controller Pak screens (4) draw no HUD, but they do draw their
    // own titles in the HUD's top-left zone with textures of the same sizes -
    // the pause menu's "Status" is one - and those must stay where they are.
    constexpr int32_t game_mode = 0x8007B2E0;
    bool hud_on_screen(uint8_t* rdram) {
        int mode = MEM_HU(0, game_mode);
        return mode != 2 && mode != 4;
    }

    int32_t alloc_sublist(size_t words) {
        int32_t bytes = static_cast<int32_t>(words * 4);
        if (sublist_cursor + bytes > sublist_ring_end) {
            sublist_cursor = sublist_ring_start;
        }
        int32_t addr = sublist_cursor;
        sublist_cursor += bytes;
        return addr;
    }

    void log_rect(int type, int ulx, int uly, int lrx, int lry, int tile = -1) {
        if (!log_rects) {
            return;
        }
        std::lock_guard lock{log_mutex};
        if (!logged_rects.emplace(type, ulx, uly, lrx, lry).second) {
            return;
        }
        std::filesystem::path path = zelda64::get_app_folder_path() / "widescreen_rects.txt";
        if (FILE* f = fopen(path.string().c_str(), "a")) {
            if (tile >= 0) {
                fprintf(f, "tex ulx=%d uly=%d lrx=%d lry=%d tile=%d tex=%dx%d %s\n", ulx, uly, lrx, lry, tile,
                    tiles[tile].width, tiles[tile].height, tiles[tile].clamp_s ? "clamp" : "wrap");
            }
            else {
                fprintf(f, "fill ulx=%d uly=%d lrx=%d lry=%d\n", ulx, uly, lrx, lry);
            }
            fclose(f);
        }
    }

    std::set<std::tuple<int, int, int, int, int, int>> logged_quads;

    // Diagnostics for 2D quads: the x/y extent, z, and colour/alpha of the
    // first vertex, written once per distinct quad.
    void log_quad(uint8_t* rdram, int32_t vtx) {
        int minx = 32767, maxx = -32768, miny = 32767, maxy = -32768;
        for (int i = 0; i < 4; i++) {
            int x = MEM_H(0, vtx + i * 16 + 0);
            int y = MEM_H(0, vtx + i * 16 + 2);
            minx = std::min(minx, x); maxx = std::max(maxx, x);
            miny = std::min(miny, y); maxy = std::max(maxy, y);
        }
        int z = MEM_H(0, vtx + 4);
        int alpha = MEM_BU(0, vtx + 15);
        int rgb = (MEM_BU(0, vtx + 12) << 16) | (MEM_BU(0, vtx + 13) << 8) | MEM_BU(0, vtx + 14);
        std::lock_guard lock{log_mutex};
        if (!logged_quads.emplace(minx, maxx, miny, maxy, z, alpha).second) {
            return;
        }
        std::filesystem::path path = zelda64::get_app_folder_path() / "widescreen_rects.txt";
        if (FILE* f = fopen(path.string().c_str(), "a")) {
            fprintf(f, "quad x=%d..%d y=%d..%d z=%d rgb=%06X a=%d\n", minx, maxx, miny, maxy, z, rgb, alpha);
            fclose(f);
        }
    }

    // Full-screen overlays (the fade to black, the dim behind the item menu)
    // are a quad from -160..160 x -120..120 in a static list, drawn under an
    // orthographic projection that RT64 keeps within the 4:3 area. The list
    // is entered through a gSPDisplayList in the frame's list; that branch is
    // redirected to a sub-list drawing the same overlay as an edge-aligned
    // fill rectangle. The game's combiner for the quad is shade * env, which
    // the sub-list reproduces with the quad's vertex colour as the primitive
    // colour, so the env-alpha fade animation still works.
    constexpr int overlay_half_width = 160;
    constexpr int screen_height = 240;

    bool is_overlay_quad(uint8_t* rdram, int32_t vtx) {
        int minx = 32767, maxx = -32768, miny = 32767, maxy = -32768;
        for (int i = 0; i < 4; i++) {
            int x = MEM_H(0, vtx + i * 16 + 0);
            int y = MEM_H(0, vtx + i * 16 + 2);
            minx = std::min(minx, x); maxx = std::max(maxx, x);
            miny = std::min(miny, y); maxy = std::max(maxy, y);
        }
        return minx == -overlay_half_width && maxx == overlay_half_width && maxy - miny >= 200;
    }

    // The game's scissor (8..312 for the whole frame) is converted to the 4:3
    // area for every draw with regular origins, so an edge-aligned rectangle
    // is clipped straight back to 4:3 unless the scissor is widened too. Each
    // sub-list therefore sets an extended full-width scissor, draws, and
    // restores the scissor the game had set (tracked while walking).
    constexpr uint32_t op_setscissor = 0xED;
    uint32_t game_scissor_w0 = 0xED020020; // 8,8 .. 312,232, the game's default
    uint32_t game_scissor_w1 = 0x004E03A0;

    // "Remove black borders" (Layout tab). The game scissors and clears an
    // inset frame, 8..312 x 8..232, while its 3D viewport (the Vp at
    // 0x80072E90) is the full 320x240, so the outer eight pixels are never
    // drawn: a black border, scaled up with the window. Rewriting that one
    // scissor and the frame clear to the full frame, in place (same-size
    // commands), lets the scene reach the edges. Other scissors and fills
    // are left alone.
    std::atomic<bool> remove_borders = false;
    constexpr uint32_t inset_scissor_w0 = 0xED020020;
    constexpr uint32_t inset_scissor_w1 = 0x004E03A0;
    constexpr uint32_t full_scissor_w0 = 0xED000000;            // 0,0
    constexpr uint32_t full_scissor_w1 = (1280u << 12) | 960u;  // 320,240 in 10.2
    constexpr uint32_t inset_clear_w0 = (0xF6u << 24) | ((311u * 4) << 12) | (231u * 4);
    constexpr uint32_t inset_clear_w1 = ((8u * 4) << 12) | (8u * 4);
    constexpr uint32_t full_clear_w0 = (0xF6u << 24) | ((319u * 4) << 12) | (239u * 4);
    constexpr uint32_t full_clear_w1 = 0x00000000;

    // Appends commands to a sub-list allocated from the ring.
    struct SubList {
        uint8_t* rdram;
        int32_t start;
        int32_t cursor;

        SubList(uint8_t* rdram_, size_t max_words) : rdram(rdram_), start(alloc_sublist(max_words)), cursor(start) {}

        void cmd(uint32_t w0, uint32_t w1) {
            write_w(rdram, cursor, w0);
            write_w(rdram, cursor + 4, w1);
            cursor += 8;
        }

        void enable_ex() {
            cmd(ex_enable_w0, ex_enable_w1);
        }

        void pipe_sync() {
            cmd(0xE7000000, 0);
        }

        // gEXSetScissorAlign changes how the *next* gDPSetScissor is placed, so
        // the game's own scissor command is re-issued after each alignment.
        // Origins LEFT/RIGHT with a -320 right offset keep the stored scissor
        // numbers as the game set them (which RT64's aspect detection relies
        // on) while pinning its edges to the real screen edges for the draw.
        void scissor_align(uint32_t lorigin, uint32_t rorigin, int lrx_offset) {
            cmd((ex_opcode << 24) | 0x000008, lorigin | (rorigin << 12));
            cmd(0, (static_cast<uint32_t>(lrx_offset * 4) & 0xFFFFu) << 16);
            cmd(0, 0xFFFFFFFFu);
        }

        void wide_scissor() {
            pipe_sync();
            scissor_align(ex_origin_left, ex_origin_right, -screen_width);
            cmd(game_scissor_w0, game_scissor_w1);
        }

        void restore_scissor() {
            pipe_sync();
            scissor_align(ex_origin_none, ex_origin_none, 0);
            cmd(game_scissor_w0, game_scissor_w1);
        }

        void end() {
            cmd(static_cast<uint32_t>(op_enddl) << 24, 0);
        }
    };

    // `branch_addr` is the gSPDisplayList command that entered the quad's list.
    void redirect_overlay_branch(uint8_t* rdram, int32_t branch_addr, int32_t vtx) {
        uint32_t rgba = (static_cast<uint32_t>(MEM_BU(0, vtx + 12)) << 24) | (static_cast<uint32_t>(MEM_BU(0, vtx + 13)) << 16)
            | (static_cast<uint32_t>(MEM_BU(0, vtx + 14)) << 8) | static_cast<uint32_t>(MEM_BU(0, vtx + 15));

        SubList sub(rdram, 40);
        sub.enable_ex();
        sub.wide_scissor();
        // gDPSetCombineLERP(PRIMITIVE, 0, ENVIRONMENT, 0, PRIMITIVE, 0, ENVIRONMENT, 0) both cycles
        sub.cmd(0xFC32BA65, 0xFF77FFFF);
        // gDPSetPrimColor with the quad's vertex colour
        sub.cmd(0xFA000000, rgba);
        // gEXFillRectangle(LEFT, RIGHT, 0, 0, 320, 240)
        sub.cmd((ex_opcode << 24) | ex_fillrect, ex_origin_left | (ex_origin_right << 12));
        sub.cmd(0, (static_cast<uint32_t>(screen_width * 4) << 16) | static_cast<uint32_t>(screen_height * 4));
        sub.restore_scissor();
        // The combiner the quad list would have left behind.
        sub.cmd(0xFC42CA85, 0xFF97FFFF);
        sub.end();

        write_w(rdram, branch_addr + 4, static_cast<uint32_t>(sub.start));
    }

    bool spans_full_width(int ulx, int lrx) {
        return ulx <= full_width_margin && lrx >= screen_width - 1 - full_width_margin;
    }

    // Fills to extend: the fades (0..336 x 0..960). The frame clear is
    // 8..311 and must stay put: extending it paints the clear colour (the
    // sky, outdoors) into the border the 3D viewport never covers.
    bool is_full_screen_fill(int ulx, int lrx) {
        return ulx == 0 && lrx >= screen_width - 1;
    }

    // Menu backdrops are drawn inset (the pause menu's strips run 23..297);
    // anything this wide made of a repeating tile is a backdrop.
    constexpr int backdrop_min_width = 256;

    bool is_backdrop_span(int ulx, int lrx) {
        return lrx - ulx >= backdrop_min_width;
    }

    // Replace the fill rectangle at `addr` with a branch to an edge-aligned copy.
    void extend_fillrect(uint8_t* rdram, int32_t addr, uint32_t w0, uint32_t w1) {
        int lry = w0 & 0xFFF;
        int uly = w1 & 0xFFF;

        SubList sub(rdram, 32);
        sub.enable_ex();
        sub.wide_scissor();
        // Coordinates are already 10.2 fixed point, which is what the command holds.
        sub.cmd((ex_opcode << 24) | ex_fillrect, ex_origin_left | (ex_origin_right << 12));
        sub.cmd(static_cast<uint32_t>(uly), (static_cast<uint32_t>(screen_width * 4) << 16) | static_cast<uint32_t>(lry));
        sub.restore_scissor();
        sub.end();

        write_w(rdram, addr, static_cast<uint32_t>(op_dl) << 24);
        write_w(rdram, addr + 4, static_cast<uint32_t>(sub.start));
    }

    // Replace the texture rectangle (three commands) at `addr` with a branch
    // to an edge-aligned copy, keeping its texture mapping.
    void extend_texrect(uint8_t* rdram, int32_t addr, uint32_t w0, uint32_t w1, uint32_t st, uint32_t dsdt) {
        int lry = w0 & 0xFFF;
        int uly = w1 & 0xFFF;
        int tile = (w1 >> 24) & 0x7;

        SubList sub(rdram, 34);
        sub.enable_ex();
        sub.wide_scissor();
        sub.cmd((ex_opcode << 24) | ex_texrect, static_cast<uint32_t>(tile) | (ex_origin_left << 3) | (ex_origin_right << 15));
        sub.cmd(static_cast<uint32_t>(uly), (static_cast<uint32_t>(screen_width * 4) << 16) | static_cast<uint32_t>(lry));
        sub.cmd(st, dsdt);
        sub.restore_scissor();
        sub.end();

        write_w(rdram, addr, static_cast<uint32_t>(op_dl) << 24);
        write_w(rdram, addr + 4, static_cast<uint32_t>(sub.start));
        // The two RDPHALF commands that carried the texture coordinates.
        write_w(rdram, addr + 8, static_cast<uint32_t>(op_spnoop) << 24);
        write_w(rdram, addr + 12, 0);
        write_w(rdram, addr + 16, static_cast<uint32_t>(op_spnoop) << 24);
        write_w(rdram, addr + 20, 0);
    }
    // Replace the texture rectangle at `addr` with a copy placed at the
    // block's position: x from the window's left edge, in frame pixels.
    void move_hud_texrect(uint8_t* rdram, int32_t addr, uint32_t w0, uint32_t w1, uint32_t st, uint32_t dsdt, const HudBlock& block, int anchor_y) {
        int lry = w0 & 0xFFF;
        int lrx = (w0 >> 12) & 0xFFF;
        int uly = w1 & 0xFFF;
        int ulx = (w1 >> 12) & 0xFFF;
        int tile = (w1 >> 24) & 0x7;
        // 10.2 fixed point throughout; nothing left of or above the window.
        // Whole pixels only: a quarter-pixel offset makes the bar's last row
        // sample the next row of the texture atlas.
        int dx = static_cast<int>(std::lround(block.x.load())) * 4;
        int dy = (static_cast<int>(std::lround(block.y.load())) - anchor_y) * 4;
        auto shift = [](int v, int d) { return static_cast<uint32_t>(std::max(v + d, 0)); };

        SubList sub(rdram, 34);
        sub.enable_ex();
        sub.wide_scissor();
        sub.cmd((ex_opcode << 24) | ex_texrect, static_cast<uint32_t>(tile) | (ex_origin_left << 3) | (ex_origin_left << 15));
        sub.cmd((shift(ulx, dx) << 16) | shift(uly, dy), (shift(lrx, dx) << 16) | shift(lry, dy));
        sub.cmd(st, dsdt);
        sub.restore_scissor();
        sub.end();

        write_w(rdram, addr, static_cast<uint32_t>(op_dl) << 24);
        write_w(rdram, addr + 4, static_cast<uint32_t>(sub.start));
        write_w(rdram, addr + 8, static_cast<uint32_t>(op_spnoop) << 24);
        write_w(rdram, addr + 12, 0);
        write_w(rdram, addr + 16, static_cast<uint32_t>(op_spnoop) << 24);
        write_w(rdram, addr + 20, 0);
    }

    // Whether this frame's widescreen 2D edits apply. The walk itself runs
    // regardless, for the object tagging below.
    bool ws_active = true;

    // Interpolation ids for objects the game draws as a plain "load modelview
    // matrix, call display list" pair in the frame's own list (map pieces,
    // trees, footprints, the water's ripple sprites...). Untagged, RT64 pairs
    // each one with a draw from the previous frame by guesswork, and when
    // effects spawn, die and reorder - the ripples do all three every few
    // frames - it pairs a dead ripple with a new one elsewhere and the
    // in-between frame draws one stretched across the water. Here each object
    // is matched to the previous frame's by display list and nearest position
    // (and similar scale), keeps that object's id, and anything new gets a new
    // id, which RT64 draws where it is without interpolating it. Characters
    // are tagged by the limb patch (func_8001DC78) and sit inside its groups,
    // so anything inside a group is left alone.
    struct TaggedObject {
        int32_t mtx_cmd;  // address of the G_MTX command (the DL call follows it)
        uint32_t dl;      // display list called
        float x, y, z;    // translation row
        float scale;      // length of the first row
        uint32_t id;
    };
    std::vector<TaggedObject> tag_current;
    std::vector<TaggedObject> tag_previous;
    uint32_t tag_next_id = 1;
    int ex_group_depth = 0;
    constexpr int32_t frame_lists_start = 0x80500000;
    constexpr int32_t frame_lists_end = 0x80600000;
    constexpr float tag_max_distance = 64.0f;
    constexpr float tag_max_scale_ratio = 1.3f;
    constexpr uint32_t tag_id_base = 0x7E000000u;

    float mtx_element(uint8_t* rdram, int32_t m, int i) {
        int32_t fixed = (static_cast<int32_t>(MEM_H(0, m + i * 2)) << 16)
            | static_cast<uint16_t>(MEM_HU(0, m + 32 + i * 2));
        return fixed / 65536.0f;
    }

    void match_and_tag_objects(uint8_t* rdram) {
        struct Pair { float cost; int cur; int prev; };
        std::vector<Pair> pairs;
        for (int c = 0; c < static_cast<int>(tag_current.size()); c++) {
            const TaggedObject& cur = tag_current[c];
            for (int p = 0; p < static_cast<int>(tag_previous.size()); p++) {
                const TaggedObject& prev = tag_previous[p];
                if (prev.dl != cur.dl) {
                    continue;
                }
                float dx = cur.x - prev.x, dy = cur.y - prev.y, dz = cur.z - prev.z;
                float dist = std::sqrt(dx * dx + dy * dy + dz * dz);
                if (dist > tag_max_distance) {
                    continue;
                }
                float lo = std::min(cur.scale, prev.scale), hi = std::max(cur.scale, prev.scale);
                if (lo <= 0.0f ? hi > 0.0f : hi / lo > tag_max_scale_ratio) {
                    continue;
                }
                pairs.push_back({ dist, c, p });
            }
        }
        std::stable_sort(pairs.begin(), pairs.end(), [](const Pair& a, const Pair& b) { return a.cost < b.cost; });
        std::vector<bool> cur_done(tag_current.size()), prev_done(tag_previous.size());
        for (const Pair& pair : pairs) {
            if (cur_done[pair.cur] || prev_done[pair.prev]) {
                continue;
            }
            tag_current[pair.cur].id = tag_previous[pair.prev].id;
            cur_done[pair.cur] = prev_done[pair.prev] = true;
        }
        for (size_t c = 0; c < tag_current.size(); c++) {
            if (!cur_done[c]) {
                tag_current[c].id = tag_id_base | (tag_next_id++ & 0xFFFFFF);
            }
        }

        // gEXMatrixGroupSimple(id, G_EX_PUSH, G_MTX_MODELVIEW, pos, rot, persp,
        // tile interpolated, vertices/texcoords/lookat skipped, linear order,
        // editable) - the same as the limb patch uses.
        constexpr uint32_t group_flags = 1u /* push */ | (1u << 3) /* pos */ | (1u << 5) /* rot */
            | (1u << 11) /* persp */ | (1u << 15) /* tile */ | (1u << 19) /* edit allow */;
        for (const TaggedObject& obj : tag_current) {
            uint32_t mtx_w0 = read_w(rdram, obj.mtx_cmd);
            uint32_t mtx_w1 = read_w(rdram, obj.mtx_cmd + 4);
            uint32_t dl_w0 = read_w(rdram, obj.mtx_cmd + 8);
            uint32_t dl_w1 = read_w(rdram, obj.mtx_cmd + 12);
            SubList sub(rdram, 14);
            sub.enable_ex();
            sub.cmd((ex_opcode << 24) | 0x00000C, obj.id);
            sub.cmd(group_flags, 0);
            sub.cmd(mtx_w0, mtx_w1);
            sub.cmd(dl_w0, dl_w1);
            sub.cmd((ex_opcode << 24) | 0x00000D, 1); // pop one modelview group
            sub.cmd(static_cast<uint32_t>(op_enddl) << 24, 0);
            write_w(rdram, obj.mtx_cmd, static_cast<uint32_t>(op_dl) << 24);
            write_w(rdram, obj.mtx_cmd + 4, static_cast<uint32_t>(sub.start));
            write_w(rdram, obj.mtx_cmd + 8, static_cast<uint32_t>(op_spnoop) << 24);
            write_w(rdram, obj.mtx_cmd + 12, 0);
        }
        tag_previous.swap(tag_current);
        tag_current.clear();
    }

    void walk(uint8_t* rdram, int32_t addr, int depth, int& budget, int32_t branch_addr = 0) {
        if (depth > max_depth) {
            return;
        }
        while (budget-- > 0) {
            // 24: a texture rectangle looks two commands ahead.
            if (!in_rdram(addr, 24)) {
                return;
            }
            uint32_t w0 = read_w(rdram, addr);
            uint32_t w1 = read_w(rdram, addr + 4);
            uint8_t op = w0 >> 24;
            dump("%08X: %02X %08X %08X\n", addr, op, w0, w1);

            switch (op) {
                case op_enddl:
                    return;

                case op_setscissor:
                    if (ws_active && remove_borders.load() && w0 == inset_scissor_w0 && w1 == inset_scissor_w1) {
                        w0 = full_scissor_w0;
                        w1 = full_scissor_w1;
                        write_w(rdram, addr, w0);
                        write_w(rdram, addr + 4, w1);
                    }
                    game_scissor_w0 = w0;
                    game_scissor_w1 = w1;
                    break;

                case op_moveword: {
                    if ((w0 & 0xFF) == mw_segment) {
                        uint32_t segment = ((w0 >> 8) & 0xFFFF) / 4;
                        if (segment < 16) {
                            segments[segment] = w1;
                        }
                    }
                    break;
                }

                case op_dl: {
                    uint32_t physical = resolve(w1);
                    int32_t target = static_cast<int32_t>(physical | 0x80000000u);
                    // Only follow lists in the game's 4MB; ours live above it
                    // and have already been handled.
                    if (physical < 0x400000) {
                        if (((w0 >> 16) & 0xFF) == 0x01) {
                            addr = target; // branch, no return
                            continue;
                        }
                        walk(rdram, target, depth + 1, budget, addr);
                    }
                    break;
                }

                case op_mtx: {
                    // F3DEX 1.x gSPMatrix: params in bits 16-23 (1 projection,
                    // 2 load, 4 push). Mtx = 16 s16 integer halves then 16 u16
                    // fractions, row-major.
                    int32_t m = static_cast<int32_t>(resolve(w1) | 0x80000000u);
                    if (dump_buf && in_rdram(m, 64)) {
                        int params = (w0 >> 16) & 0xFF;
                        dump("    mtx %s%s%s\n", (params & 1) ? "proj" : "modelview",
                            (params & 2) ? " load" : " mul", (params & 4) ? " push" : "");
                        for (int row = 0; row < 4; row++) {
                            dump("     ");
                            for (int col = 0; col < 4; col++) {
                                int i = row * 4 + col;
                                int32_t fixed = (static_cast<int32_t>(MEM_H(0, m + i * 2)) << 16)
                                    | static_cast<uint16_t>(MEM_HU(0, m + 32 + i * 2));
                                dump(" %12.5f", fixed / 65536.0);
                            }
                            dump("\n");
                        }
                    }
                    // A modelview load in the frame's own list, outside any
                    // id group, followed directly by a display list call (not
                    // a branch): an object to tag after the walk.
                    uint32_t next_w0 = read_w(rdram, addr + 8);
                    int params = (w0 >> 16) & 0xFF;
                    if ((params & 1) == 0 && (params & 2) != 0 && ex_group_depth == 0
                        && addr >= frame_lists_start && addr + 16 <= frame_lists_end
                        && (next_w0 >> 24) == op_dl && ((next_w0 >> 16) & 0xFF) == 0x00
                        && in_rdram(m, 64)) {
                        float r0x = mtx_element(rdram, m, 0), r0y = mtx_element(rdram, m, 1), r0z = mtx_element(rdram, m, 2);
                        tag_current.push_back({ addr, resolve(read_w(rdram, addr + 12)),
                            mtx_element(rdram, m, 12), mtx_element(rdram, m, 13), mtx_element(rdram, m, 14),
                            std::sqrt(r0x * r0x + r0y * r0y + r0z * r0z), 0 });
                    }
                    break;
                }

                case ex_opcode: {
                    // RT64 extended commands are two words long when they are
                    // a matrix group; track the group depth so objects already
                    // tagged (the limb patch) are not tagged again.
                    uint32_t sub_op = w0 & 0xFFFFFF;
                    if (sub_op == 0x00000C) {
                        if (read_w(rdram, addr + 8) & 1) {
                            ex_group_depth++;
                        }
                        addr += 8;
                    }
                    else if (sub_op == 0x00000D) {
                        ex_group_depth = std::max(0, ex_group_depth - static_cast<int>(w1 & 0xFF));
                    }
                    break;
                }

                case op_vtx: {
                    // F3DEX 1.x gSPVertex: w0 = 04 | v0*2 << 16 | n << 10 | (16n-1).
                    int n = (w0 >> 10) & 0x3F;
                    if (!in_rdram(static_cast<int32_t>(resolve(w1) | 0x80000000u), n * 16)) {
                        break;
                    }
                    if (dump_buf) {
                        int32_t v = static_cast<int32_t>(resolve(w1) | 0x80000000u);
                        for (int i = 0; i < n; i++) {
                            dump("    v%d: x=%d y=%d z=%d rgba=%02X%02X%02X%02X\n", i,
                                (int)MEM_H(0, v + i * 16), (int)MEM_H(0, v + i * 16 + 2), (int)MEM_H(0, v + i * 16 + 4),
                                (int)MEM_BU(0, v + i * 16 + 12), (int)MEM_BU(0, v + i * 16 + 13),
                                (int)MEM_BU(0, v + i * 16 + 14), (int)MEM_BU(0, v + i * 16 + 15));
                        }
                    }
                    if (n == 4) {
                        int32_t v = static_cast<int32_t>(resolve(w1) | 0x80000000u);
                        if (log_rects) {
                            log_quad(rdram, v);
                        }
                        if (ws_active && branch_addr != 0 && is_overlay_quad(rdram, v)) {
                            redirect_overlay_branch(rdram, branch_addr, v);
                            // The rest of the quad's list is no longer drawn.
                            return;
                        }
                    }
                    break;
                }

                case op_settilesize: {
                    int tile = (w1 >> 24) & 0x7;
                    tiles[tile].width = ((((w1 >> 12) & 0xFFF) - ((w0 >> 12) & 0xFFF)) >> 2) + 1;
                    tiles[tile].height = (((w1 & 0xFFF) - (w0 & 0xFFF)) >> 2) + 1;
                    break;
                }

                case op_settile: {
                    int tile = (w1 >> 24) & 0x7;
                    // cms is bits 8-9 of w1; bit 9 set means clamp.
                    tiles[tile].clamp_s = ((w1 >> 8) & 0x2) != 0;
                    break;
                }

                case op_fillrect: {
                    if (ws_active && remove_borders.load() && w0 == inset_clear_w0 && w1 == inset_clear_w1) {
                        w0 = full_clear_w0;
                        w1 = full_clear_w1;
                        write_w(rdram, addr, w0);
                        write_w(rdram, addr + 4, w1);
                    }
                    int lrx = (w0 >> 12) & 0xFFF;
                    int ulx = (w1 >> 12) & 0xFFF;
                    log_rect(op, ulx >> 2, (w1 & 0xFFF) >> 2, lrx >> 2, (w0 & 0xFFF) >> 2);
                    if (ws_active && is_full_screen_fill(ulx >> 2, lrx >> 2)) {
                        extend_fillrect(rdram, addr, w0, w1);
                    }
                    break;
                }

                case op_texrect:
                case op_texrectflip: {
                    int lrx = (w0 >> 12) & 0xFFF;
                    int ulx = (w1 >> 12) & 0xFFF;
                    uint32_t h1_w0 = read_w(rdram, addr + 8);
                    uint32_t h2_w0 = read_w(rdram, addr + 16);
                    int tile = (w1 >> 24) & 0x7;
                    log_rect(op, ulx >> 2, (w1 & 0xFFF) >> 2, lrx >> 2, (w0 & 0xFFF) >> 2, tile);
                    bool backdrop = tiles[tile].width > 0 && tiles[tile].width <= backdrop_max_texture_width;
                    if (ws_active && op == op_texrect && (h1_w0 >> 24) == op_rdphalf_1 && (h2_w0 >> 24) == op_rdphalf_2
                        && backdrop && is_backdrop_span(ulx >> 2, lrx >> 2)) {
                        extend_texrect(rdram, addr, w0, w1, read_w(rdram, addr + 12), read_w(rdram, addr + 20));
                    }
                    else if (ws_active && op == op_texrect && (h1_w0 >> 24) == op_rdphalf_1 && (h2_w0 >> 24) == op_rdphalf_2) {
                        HudBlock* block = hud_block_of(ulx >> 2, (w1 & 0xFFF) >> 2, tiles[tile]);
                        if (block != nullptr && block->custom.load() && hud_on_screen(rdram)) {
                            move_hud_texrect(rdram, addr, w0, w1, read_w(rdram, addr + 12), read_w(rdram, addr + 20), *block,
                                block == &hud_hp ? hud_hp_anchor_y : hud_sp_anchor_y);
                        }
                    }
                    // Skip the two RDPHALF commands regardless.
                    addr += 16;
                    break;
                }

                default:
                    break;
            }
            addr += 8;
        }
    }
}

void zelda64::renderer::set_widescreen_2d_enabled(bool value) {
    enabled.store(value);
}

void zelda64::renderer::set_borders_removed(bool value) {
    remove_borders.store(value);
}

void zelda64::renderer::set_hud_layout(bool hp_custom, float hp_x, float hp_y, bool sp_custom, float sp_x, float sp_y) {
    hud_hp.x.store(hp_x);
    hud_hp.y.store(hp_y);
    hud_hp.custom.store(hp_custom);
    hud_spirits.x.store(sp_x);
    hud_spirits.y.store(sp_y);
    hud_spirits.custom.store(sp_custom);
}

void zelda64::renderer::request_widescreen_frame_dump() {
    capture_save_requested.store(true);
}

// Hooked in nnScExecuteGraphics just before osSpTaskLoad, with the NUScTask
// in a0; the OSTask follows at +0x10 and its display list pointer at +0x30.
extern "C" void quest64_widescreen_task(uint8_t* rdram, recomp_context* ctx) {
    ws_active = enabled.load();
    int32_t task = static_cast<int32_t>(ctx->r4) + 0x10;
    if (!in_rdram(task + 0x30, 4)) {
        return;
    }
    uint32_t data_ptr = read_w(rdram, task + 0x30);
    if (data_ptr == 0) {
        return;
    }
    if (capture_enabled < 0) {
        capture_enabled = std::filesystem::exists(zelda64::get_app_folder_path() / "frame_capture.txt") ? 1 : 0;
    }
    if (capture_enabled) {
        dump_buf = &capture_ring[capture_next];
        dump_buf->clear();
        dump("frame at %llu ms\n", static_cast<unsigned long long>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch()).count()));
    }
    for (uint32_t& segment : segments) {
        segment = 0;
    }
    game_scissor_w0 = 0xED020020;
    game_scissor_w1 = 0x004E03A0;
    int budget = max_commands;
    ex_group_depth = 0;
    tag_current.clear();
    walk(rdram, static_cast<int32_t>(data_ptr | 0x80000000u), 0, budget);
    match_and_tag_objects(rdram);
    if (dump_buf) {
        dump_buf = nullptr;
        capture_next = (capture_next + 1) % capture_frames;
        capture_count = std::min(capture_count + 1, capture_frames);
        if (capture_save_requested.exchange(false)) {
            save_capture();
        }
    }
}
