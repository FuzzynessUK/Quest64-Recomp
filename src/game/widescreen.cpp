#include <algorithm>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <mutex>
#include <set>
#include <tuple>

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
    constexpr uint8_t op_vtx = 0x04;

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

    constexpr uint32_t ex_opcode = 0x64;
    constexpr uint32_t ex_fillrect = 0x000003;
    constexpr uint32_t ex_texrect = 0x000002;
    constexpr uint32_t ex_origin_left = 0x0;
    constexpr uint32_t ex_origin_right = 0x400;
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

    // Diagnostics: every distinct rectangle seen, written once.
    std::mutex log_mutex;
    std::set<std::tuple<int, int, int, int, int>> logged_rects;
    bool log_rects = true;

    uint32_t read_w(uint8_t* rdram, int32_t addr) {
        return static_cast<uint32_t>(MEM_W(0, addr));
    }

    void write_w(uint8_t* rdram, int32_t addr, uint32_t value) {
        MEM_W(0, addr) = static_cast<int32_t>(value);
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

    bool spans_full_width(int ulx, int lrx) {
        return ulx <= full_width_margin && lrx >= screen_width - 1 - full_width_margin;
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

        int32_t sub = alloc_sublist(8);
        write_w(rdram, sub + 0, ex_enable_w0);
        write_w(rdram, sub + 4, ex_enable_w1);
        write_w(rdram, sub + 8, (ex_opcode << 24) | ex_fillrect);
        write_w(rdram, sub + 12, ex_origin_left | (ex_origin_right << 12));
        // Already 10.2 fixed point, which is what the command holds.
        write_w(rdram, sub + 16, (0u << 16) | static_cast<uint32_t>(uly));
        write_w(rdram, sub + 20, (static_cast<uint32_t>(screen_width * 4) << 16) | static_cast<uint32_t>(lry));
        write_w(rdram, sub + 24, static_cast<uint32_t>(op_enddl) << 24);
        write_w(rdram, sub + 28, 0);

        write_w(rdram, addr, static_cast<uint32_t>(op_dl) << 24);
        write_w(rdram, addr + 4, static_cast<uint32_t>(sub));
    }

    // Replace the texture rectangle (three commands) at `addr` with a branch
    // to an edge-aligned copy, keeping its texture mapping.
    void extend_texrect(uint8_t* rdram, int32_t addr, uint32_t w0, uint32_t w1, uint32_t st, uint32_t dsdt) {
        int lry = w0 & 0xFFF;
        int uly = w1 & 0xFFF;
        int tile = (w1 >> 24) & 0x7;

        int32_t sub = alloc_sublist(10);
        write_w(rdram, sub + 0, ex_enable_w0);
        write_w(rdram, sub + 4, ex_enable_w1);
        write_w(rdram, sub + 8, (ex_opcode << 24) | ex_texrect);
        write_w(rdram, sub + 12, static_cast<uint32_t>(tile) | (ex_origin_left << 3) | (ex_origin_right << 15));
        write_w(rdram, sub + 16, (0u << 16) | static_cast<uint32_t>(uly));
        write_w(rdram, sub + 20, (static_cast<uint32_t>(screen_width * 4) << 16) | static_cast<uint32_t>(lry));
        write_w(rdram, sub + 24, st);
        write_w(rdram, sub + 28, dsdt);
        write_w(rdram, sub + 32, static_cast<uint32_t>(op_enddl) << 24);
        write_w(rdram, sub + 36, 0);

        write_w(rdram, addr, static_cast<uint32_t>(op_dl) << 24);
        write_w(rdram, addr + 4, static_cast<uint32_t>(sub));
        // The two RDPHALF commands that carried the texture coordinates.
        write_w(rdram, addr + 8, static_cast<uint32_t>(op_spnoop) << 24);
        write_w(rdram, addr + 12, 0);
        write_w(rdram, addr + 16, static_cast<uint32_t>(op_spnoop) << 24);
        write_w(rdram, addr + 20, 0);
    }

    void walk(uint8_t* rdram, int32_t addr, int depth, int& budget) {
        if (depth > max_depth) {
            return;
        }
        while (budget-- > 0) {
            uint32_t w0 = read_w(rdram, addr);
            uint32_t w1 = read_w(rdram, addr + 4);
            uint8_t op = w0 >> 24;

            switch (op) {
                case op_enddl:
                    return;

                case op_dl: {
                    uint32_t physical = w1 & 0x1FFFFFFFu;
                    int32_t target = static_cast<int32_t>(physical | 0x80000000u);
                    // Only follow lists in the game's 4MB; ours live above it
                    // and have already been handled.
                    if (physical < 0x400000) {
                        if (((w0 >> 16) & 0xFF) == 0x01) {
                            addr = target; // branch, no return
                            continue;
                        }
                        walk(rdram, target, depth + 1, budget);
                    }
                    break;
                }

                case op_vtx: {
                    // gSPVertex: n-1 in bits 20-23, address in w1. Only quads
                    // are of interest (2D overlays drawn as two triangles).
                    int n = ((w0 >> 20) & 0xF) + 1;
                    if (n == 4 && log_rects) {
                        log_quad(rdram, static_cast<int32_t>((w1 & 0x1FFFFFFFu) | 0x80000000u));
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
                    int lrx = (w0 >> 12) & 0xFFF;
                    int ulx = (w1 >> 12) & 0xFFF;
                    log_rect(op, ulx >> 2, (w1 & 0xFFF) >> 2, lrx >> 2, (w0 & 0xFFF) >> 2);
                    if (spans_full_width(ulx >> 2, lrx >> 2)) {
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
                    if (op == op_texrect && (h1_w0 >> 24) == op_rdphalf_1 && (h2_w0 >> 24) == op_rdphalf_2
                        && backdrop && is_backdrop_span(ulx >> 2, lrx >> 2)) {
                        extend_texrect(rdram, addr, w0, w1, read_w(rdram, addr + 12), read_w(rdram, addr + 20));
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

// Hooked in nnScExecuteGraphics just before osSpTaskLoad, with the NUScTask
// in a0; the OSTask follows at +0x10 and its display list pointer at +0x30.
extern "C" void quest64_widescreen_task(uint8_t* rdram, recomp_context* ctx) {
    if (!enabled.load()) {
        return;
    }
    int32_t task = static_cast<int32_t>(ctx->r4) + 0x10;
    uint32_t data_ptr = read_w(rdram, task + 0x30);
    if (data_ptr == 0) {
        return;
    }
    int budget = max_commands;
    walk(rdram, static_cast<int32_t>(data_ptr | 0x80000000u), 0, budget);
}
