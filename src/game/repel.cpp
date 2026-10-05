#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <map>
#include <span>
#include <string>
#include <vector>

#include "repel.h"
#include "pageitem.h"
#include "hardmode.h"
#include "enhancements.h"
#include "notify.h"
#include "speedrun.h"
#include "zelda_config.h"
#include "json/json.hpp"
#include "librecomp/game.hpp"
#include "librecomp/addresses.hpp"
#include "recomp.h"

using json = nlohmann::json;

extern "C" void func_8000669C(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_80014A98(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_800210FC(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_80020E2C(uint8_t* rdram, recomp_context* ctx);
extern "C" void func_80020F8C(uint8_t* rdram, recomp_context* ctx);

namespace {
    constexpr uint32_t icon_rom = 0xD3C240 + zelda64::repel::item_id * 0x100;
    constexpr int32_t name_table = 0x803A9954;    // u32 pointer per item, 32 slots
    constexpr int32_t desc_table = 0x803A99D4;
    // Item 25's name pointer as the game loads it: the tables are only
    // touched once they are in RAM (as in pageitem.cpp).
    constexpr int32_t dark_gaol_key_name = 0x803A94C4;

    constexpr int32_t brian = 0x8007BACC;         // +0x68: status block
    constexpr int32_t menu_mask = 0x8007B2E4;     // bit 0x800: no status icons
    constexpr int32_t gBattleState = 0x8008C592;  // bit 0: a battle is running
    constexpr int32_t gNextMap = 0x80084EE4;      // -1 until a save is loaded
    constexpr int32_t gInventory = 0x8008CF78;
    constexpr int inventory_slots = 150;
    constexpr int inventory_empty = 0xFF;
    constexpr int32_t item_menu_page = 0x8008C760;
    constexpr int32_t item_menu_cursor = 0x8008C764;

    // Spirit Armor Lv1, the Silver Amulet's spell: Earth (1), entry 2. The
    // spell records are 0x44 bytes, in one array per element whose base is
    // the word at 0x800C1B14 + element * 4 (func_80014A98).
    constexpr int spell_element = 1;
    constexpr int spell_index = 2;
    constexpr int32_t spell_tables = 0x800C1B14;
    constexpr int spell_record_size = 0x44;
    // The status parameter block, one byte per status effect (HANDOFF.md,
    // "The spell effect parameter block"); Spirit Armor's is +0x3C.
    constexpr int status_params_first = 0x34;
    constexpr int status_params_last = 0x3F;

    bool state_on = false;
    bool was_on_title = false;
    // The bag slot of a Repel being used, from the use hook until the menu
    // has taken it out and it has been put back.
    int keep_slot = -1;

    int32_t name_addr = 0;
    int32_t desc_addr = 0;
    constexpr size_t desc_room = 48;
    int shown_state = -1;

    // The head icon: the CI8 pixels and the descriptor func_800210FC reads
    // (+0 address, +8 width, +0xC height, as the status atlas's at
    // 0x803A6F70).
    int32_t head_pixels = 0;
    int32_t head_desc = 0;

    int32_t spell_copy = 0;
    // What Brian's status block held when Repel was cast, put back if the
    // spell sets anything in it after all; counted down a frame at a time.
    int guard_frames = 0;
    int32_t guard_block = 0;
    std::array<uint8_t, 0x42> guard_bytes{};
    bool guard_noted = false;

    std::map<std::string, bool> by_save;
    bool saves_loaded = false;

    std::filesystem::path saves_path() {
        return zelda64::get_app_folder_path() / "repel_saves.json";
    }

    void log_line(const std::string& line) {
        std::ofstream out(zelda64::get_app_folder_path() / "repel.txt", std::ios::app);
        out << line << "\n";
    }

    void load_saves() {
        if (saves_loaded) {
            return;
        }
        saves_loaded = true;
        std::ifstream in(saves_path());
        if (!in.is_open()) {
            return;
        }
        json j = json::parse(in, nullptr, false);
        if (!j.is_object()) {
            return;
        }
        for (auto it = j.begin(); it != j.end(); ++it) {
            if (it.value().is_boolean()) {
                by_save[it.key()] = it.value().get<bool>();
            }
        }
    }

    // A block of librecomp's heap, as a KSEG0 address, 8-byte aligned for
    // the texture loads.
    int32_t heap_block(uint8_t* rdram, size_t size) {
        void* mem = recomp::alloc(rdram, size + 8);
        if (mem == nullptr) {
            return 0;
        }
        uint32_t addr = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(mem) - rdram) + 0x80000000u;
        return static_cast<int32_t>((addr + 7) & ~7u);
    }

    void write_bytes(uint8_t* rdram, int32_t addr, const std::vector<uint8_t>& bytes) {
        for (size_t i = 0; i < bytes.size(); i++) {
            MEM_B(static_cast<int32_t>(i), addr) = static_cast<int8_t>(bytes[i]);
        }
    }

    bool in_battle(uint8_t* rdram) {
        return (MEM_HU(0, gBattleState) & 1) != 0;
    }

    // Brian casts Spirit Armor from a copy of its record with the status
    // parameters zeroed: the effect and the sound, and no defence. The
    // element's table pointer is moved for the one call so that every read
    // the cast makes lands on the copy; the instance it starts keeps the
    // copy's address. As Brian's own casts call it (func_800045F0): a2 0,
    // a3 0.
    void cast(uint8_t* rdram, recomp_context* ctx, int32_t actor) {
        if (spell_copy == 0) {
            spell_copy = heap_block(rdram, spell_record_size);
            if (spell_copy == 0) {
                log_line("no room for the spell copy; no cast");
                return;
            }
        }
        const int32_t table = spell_tables + spell_element * 4;
        const int32_t base = MEM_W(0, table);
        const int32_t original = base + spell_index * spell_record_size;
        for (int i = 0; i < spell_record_size; i++) {
            bool status = i >= status_params_first && i <= status_params_last;
            MEM_B(i, spell_copy) = status ? 0 : MEM_B(i, original);
        }

        int32_t block = MEM_W(0x68, actor);
        if (block != 0) {
            guard_block = block;
            for (size_t i = 0; i < guard_bytes.size(); i++) {
                guard_bytes[i] = static_cast<uint8_t>(MEM_BU(static_cast<int32_t>(i), block));
            }
            guard_frames = 300;
            guard_noted = false;
        }

        MEM_W(0, table) = spell_copy - spell_index * spell_record_size;
        recomp_context c = *ctx;
        c.r29 = ADD32(ctx->r29, -0x80);
        c.r4 = actor;
        c.r5 = (spell_element << 8) | spell_index;
        c.r6 = 0;
        c.r7 = 0;
        func_80014A98(rdram, &c);
        MEM_W(0, table) = base;
    }

    // Puts Brian's status block back if the cast set a status in it after
    // all. Stops at the first battle, which brings statuses of its own.
    void guard_status(uint8_t* rdram) {
        if (guard_frames <= 0) {
            return;
        }
        if (in_battle(rdram) || MEM_W(0x68, brian) != guard_block) {
            guard_frames = 0;
            return;
        }
        guard_frames--;
        bool changed = false;
        // The status bits (u16 at +0) and the per-status bytes up to the
        // visual slots at +0x44, which are the game's to tidy away once the
        // bits are gone (func_8001FEEC).
        for (size_t i = 0; i < guard_bytes.size(); i++) {
            if (static_cast<uint8_t>(MEM_BU(static_cast<int32_t>(i), guard_block)) != guard_bytes[i]) {
                changed = true;
                MEM_B(static_cast<int32_t>(i), guard_block) = static_cast<int8_t>(guard_bytes[i]);
            }
        }
        if (changed && !guard_noted) {
            guard_noted = true;
            log_line("the cast set a status after all; put back");
        }
    }

    void give_if_missing(uint8_t* rdram) {
        if (!zelda64::enhancements::active_options().repel ||
            zelda64::enhancements::item_menu_open(rdram) ||
            zelda64::enhancements::bag_count(rdram, zelda64::repel::item_id) > 0) {
            return;
        }
        for (int slot = 0; slot < inventory_slots; slot++) {
            if (MEM_BU(slot, gInventory) == inventory_empty) {
                MEM_B(slot, gInventory) = static_cast<int8_t>(zelda64::repel::item_id);
                return;
            }
        }
    }
}

bool zelda64::repel::available() {
    return !zelda64::hardmode::active();
}

bool zelda64::repel::active() {
    return state_on && available();
}

void zelda64::repel::apply_at_boot() {
    if (!available()) {
        return;
    }
    // Into the ROM image in place: the menu DMAs each icon from it as it is
    // shown (pageitem.cpp does the same).
    std::span<const uint8_t> rom = recomp::get_rom();
    if (rom.size() >= icon_rom + bag_icon.size()) {
        std::memcpy(const_cast<uint8_t*>(rom.data()) + icon_rom, bag_icon.data(), bag_icon.size());
    }
}

void zelda64::repel::save_progress(const std::string& save_key) {
    load_saves();
    by_save[save_key] = state_on;
    json j = json::object();
    for (const auto& [key, on] : by_save) {
        j[key] = on;
    }
    std::ofstream out(saves_path());
    if (out.is_open()) {
        out << j.dump();
    }
}

void zelda64::repel::load_progress(const std::string& save_key) {
    load_saves();
    auto it = by_save.find(save_key);
    state_on = it != by_save.end() && it->second;
}

void zelda64::repel::on_frame(uint8_t* rdram) {
    keep_slot = -1;
    // Back on the title: a new game starts with it off, and a file loaded
    // brings its own (load_progress, which runs while the title is up).
    bool on_title = zelda64::speedrun::title_showing();
    if (on_title && !was_on_title) {
        state_on = false;
    }
    was_on_title = on_title;

    if (!available() || MEM_W(25 * 4, name_table) != dark_gaol_key_name) {
        return;
    }
    if (name_addr == 0) {
        name_addr = heap_block(rdram, 16);
        desc_addr = heap_block(rdram, desc_room);
        if (name_addr != 0) {
            write_bytes(rdram, name_addr, zelda64::page_item::encode_text("repel"));
        }
    }
    if (name_addr == 0 || desc_addr == 0) {
        return;
    }
    if (MEM_W(item_id * 4, name_table) != name_addr) {
        MEM_W(item_id * 4, name_table) = name_addr;
        MEM_W(item_id * 4, desc_table) = desc_addr;
    }
    if (shown_state != static_cast<int>(state_on)) {
        shown_state = static_cast<int>(state_on);
        std::vector<uint8_t> text = zelda64::page_item::encode_text(
            state_on ? "repels monsters\nactivated" : "repels monsters\nnot active");
        text.resize(std::min(text.size(), desc_room));
        text.back() = 0xFF;
        write_bytes(rdram, desc_addr, text);
    }

    guard_status(rdram);
    if (!on_title && MEM_W(0, gNextMap) != -1) {
        give_if_missing(rdram);
    }
}

// func_800212E4 at its first instruction, a0 the item: "use this item".
// Repel's properties record does not exist, so it is answered here. In the
// field Brian takes up the item-use pose, as func_800212E4 itself does for
// any usable item, and the answer is yes; in battle it is no.
extern "C" int quest64_repel_use(uint8_t* rdram, recomp_context* ctx) {
    if (!zelda64::repel::available() || (ctx->r4 & 0xFF) != zelda64::repel::item_id) {
        return 0;
    }
    if (in_battle(rdram)) {
        ctx->r2 = 0;
        return 1;
    }
    recomp_context c = *ctx;
    c.r29 = ADD32(ctx->r29, -0x40);
    c.r4 = zelda64::repel::item_id;
    func_8000669C(rdram, &c);
    keep_slot = static_cast<int32_t>(MEM_W(0, item_menu_page)) + static_cast<int32_t>(MEM_W(0, item_menu_cursor));
    ctx->r2 = 1;
    return 1;
}

// func_80021524 at 0x80022178: the menu has taken the used item out of the
// bag (every byte from its slot to the last of the 150 moved down one, 0xFF
// written to the last). Undoing exactly that puts Repel back where it was,
// whichever layout the bag is in (Stack Items keeps its counts up there).
extern "C" void quest64_repel_keep(uint8_t* rdram, recomp_context* ctx) {
    (void)ctx;
    if (keep_slot < 0 || keep_slot >= inventory_slots) {
        return;
    }
    int slot = keep_slot;
    keep_slot = -1;
    for (int i = inventory_slots - 1; i > slot; i--) {
        MEM_B(i, gInventory) = MEM_B(i - 1, gInventory);
    }
    MEM_B(slot, gInventory) = static_cast<int8_t>(zelda64::repel::item_id);
}

// func_800213D8 at its first instruction, a0 the item and a1 the actor: the
// use takes effect as Brian's item pose ends (func_80004AB8).
extern "C" int quest64_repel_apply(uint8_t* rdram, recomp_context* ctx) {
    if (!zelda64::repel::available() || (ctx->r4 & 0xFF) != zelda64::repel::item_id) {
        return 0;
    }
    state_on = !state_on;
    zelda64::notify::post(state_on ? "Repel activated" : "Repel deactivated");
    log_line(state_on ? "activated" : "deactivated");
    cast(rdram, ctx, static_cast<int32_t>(ctx->r5));
    ctx->r2 = 1;
    return 1;
}

// func_8001E25C's encounter check, func_8001C5F4: skipped while Repel is on.
extern "C" int quest64_repel_active() {
    return zelda64::repel::active() ? 1 : 0;
}

namespace {
    constexpr int head_size = 16;
    constexpr int32_t display_list = 0x8007B2FC;   // the HUD's DL write pointer

    // The icon's pixels and the descriptor func_800210FC reads. Not in
    // librecomp's heap like the rest of this file's blocks: that is above the
    // 8MB the RDP sees, and a texture loaded from there draws nothing. The
    // game assumes 4MB and never asks, so the top of the 8MB is free; the
    // widescreen code keeps its display-list ring at 0x807E0000 up, and
    // these sit just under it. Written whenever they are not what they
    // should be, which is once.
    constexpr int32_t head_pixels_addr = 0x807DFE00;
    constexpr int32_t head_desc_addr = 0x807DFF00;
    bool head_ready(uint8_t* rdram) {
        head_pixels = head_pixels_addr;
        head_desc = head_desc_addr;
        if (MEM_W(0x0, head_desc) != head_pixels || MEM_W(0x8, head_desc) != head_size) {
            write_bytes(rdram, head_pixels, std::vector<uint8_t>(zelda64::repel::head_icon.begin(), zelda64::repel::head_icon.end()));
            MEM_W(0x0, head_desc) = head_pixels;
            MEM_W(0x4, head_desc) = 0;
            MEM_W(0x8, head_desc) = head_size;
            MEM_W(0xC, head_desc) = head_size;
        }
        return true;
    }

    // The icon is drawn at half its texture size: the whole 16x16 picture in
    // an 8x8 box, texture step 2.0, so at the renderer's resolution every
    // texel still shows. func_800210FC ties the box to the texture size, so
    // its two halves are called directly, as it calls them:
    //   func_80020E2C(desc, s, t, w, [sp+0x10] h) loads the texels;
    //   func_80020F8C(x, y, w, h, [sp+0x10] s, t, dsdx, dtdy) draws the box
    //   (1.0 is 0x400). x, y here are the icon's centre.
    constexpr int head_drawn = 8;
    void draw_head(uint8_t* rdram, recomp_context* ctx, int32_t x, int32_t y) {
        recomp_context c = *ctx;
        c.r29 = ADD32(ctx->r29, -0x40);
        MEM_W(0x10, c.r29) = head_size;
        c.r4 = head_desc;
        c.r5 = 0;
        c.r6 = 0;
        c.r7 = head_size;
        func_80020E2C(rdram, &c);

        c = *ctx;
        c.r29 = ADD32(ctx->r29, -0x40);
        constexpr int32_t step = 0x400 * head_size / head_drawn;
        MEM_W(0x10, c.r29) = 0;
        MEM_W(0x14, c.r29) = 0;
        MEM_W(0x18, c.r29) = step;
        MEM_W(0x1C, c.r29) = step;
        c.r4 = x - head_drawn / 2;
        c.r5 = y - head_drawn / 2;
        c.r6 = head_drawn;
        c.r7 = head_drawn;
        func_80020F8C(rdram, &c);
    }

    void dl(uint8_t* rdram, uint32_t w0, uint32_t w1) {
        int32_t p = MEM_W(0, display_list);
        MEM_W(0, p) = static_cast<int32_t>(w0);
        MEM_W(4, p) = static_cast<int32_t>(w1);
        MEM_W(0, display_list) = p + 8;
    }

    float mem_f(uint8_t* rdram, int32_t addr) {
        uint32_t bits = static_cast<uint32_t>(MEM_W(0, addr));
        float f;
        std::memcpy(&f, &bits, sizeof f);
        return f;
    }
}


// In battle. func_8001FEEC at 0x800202B4, once it has drawn an actor's
// status icons and before it returns. For Brian, Repel's icon goes next in
// the column: the routine centres each icon on the head point it projected
// (sp+0xB0 x, sp+0xAC y) plus a running offset, s7 across and s6 up, which
// still holds where the next one would go. t4 is the projection's "on
// screen" answer. The status palette is in TMEM already; the routine loads
// it unless menu bit 0x800 is set, in which case it draws no icons and
// neither does this.
extern "C" void quest64_repel_head_icon(uint8_t* rdram, recomp_context* ctx) {
    if (!zelda64::repel::active() || static_cast<int32_t>(ctx->r19) != brian ||
        (MEM_W(0, menu_mask) & 0x800) != 0 || static_cast<int32_t>(ctx->r12) == 0 ||
        !head_ready(rdram)) {
        return;
    }
    int32_t sp = static_cast<int32_t>(ctx->r29);
    draw_head(rdram, ctx,
              static_cast<int32_t>(MEM_W(0xB0, sp)) + static_cast<int32_t>(ctx->r23),
              static_cast<int32_t>(MEM_W(0xAC, sp)) + static_cast<int32_t>(ctx->r22));
}

// In the field. func_8001FEEC only runs in battle - the HUD, func_8001E25C,
// tests gBattleState bit 0 at 0x8001E65C and takes the field branch at
// 0x8001E758 otherwise, which is where this sits - so the field gets what
// that routine does for one icon: Brian's head point (the same sum it
// makes: his position, y raised by the height at +0x1C of what +0x64
// points to times his scale at +0x24) projected onto the screen, the
// status palette loaded into TMEM exactly as it loads it, palette lookup on
// (G_TT_RGBA16, which the field's own overhead marker, func_8001F818,
// also sets and leaves), and the icon drawn. Hidden while menu bits 0-2
// are up, as that marker is.
namespace {
    // For finding out why the icon is not where it should be: what the field
    // draw saw, every two seconds while Repel is on.
    int field_note_frames = 0;
    void field_note(const std::string& what) {
        if (field_note_frames > 0) {
            return;
        }
        field_note_frames = 120;
        log_line("field icon: " + what);
    }
}

extern "C" void quest64_repel_field_icon(uint8_t* rdram, recomp_context* ctx) {
    if (!zelda64::repel::active()) {
        return;
    }
    if (field_note_frames > 0) {
        field_note_frames--;
    }
    uint32_t mask = static_cast<uint32_t>(MEM_W(0, menu_mask));
    if ((mask & 0x7) != 0 || !head_ready(rdram)) {
        char note[64];
        std::snprintf(note, sizeof note, "skipped, menu mask %08X", mask);
        field_note(note);
        return;
    }
    int32_t model = MEM_W(0x64, brian);
    if (model == 0) {
        return;
    }
    float x = mem_f(rdram, brian + 0x0);
    float y = mem_f(rdram, brian + 0x4) + mem_f(rdram, model + 0x1C) * mem_f(rdram, brian + 0x24);
    if (MEM_HU(0, model) == 1) {
        int32_t block = MEM_W(0x68, brian);
        if (block == 0) {
            return;
        }
        y = mem_f(rdram, block + 0x94);
    }

    // Projected through the camera's own matrices, not func_8002413C: that
    // routine does the perspective by hand with the battle camera's numbers,
    // and in the field it pulls the point towards the middle of the screen,
    // further the further Brian is from it. libultra row vectors, clip =
    // world * view (0x80086E88) * projection (0x80086E48), the pair the
    // field camera builds (statfx.cpp projects Brian the same way).
    float z = mem_f(rdram, brian + 0x8);
    float view[4], clip[4];
    const float in[4] = { x, y, z, 1.0f };
    for (int col = 0; col < 4; col++) {
        view[col] = 0.0f;
        for (int row = 0; row < 4; row++) {
            view[col] += in[row] * mem_f(rdram, 0x80086E88 + (row * 4 + col) * 4);
        }
    }
    for (int col = 0; col < 4; col++) {
        clip[col] = 0.0f;
        for (int row = 0; row < 4; row++) {
            clip[col] += view[row] * mem_f(rdram, 0x80086E48 + (row * 4 + col) * 4);
        }
    }
    if (!(clip[3] > 0.0001f)) {
        field_note("behind the camera");
        return;
    }
    // NDC to the game's 320x240 frame.
    const int32_t screen_x = static_cast<int32_t>(160.0f + 160.0f * clip[0] / clip[3]);
    const int32_t screen_y = static_cast<int32_t>(120.0f - 120.0f * clip[1] / clip[3]);
    {
        char note[128];
        std::snprintf(note, sizeof note, "head (%.1f, %.1f, %.1f) at %d,%d", x, y, z, screen_x, screen_y);
        field_note(note);
    }
    if (screen_x < -16 || screen_x > 336 || screen_y < -16 || screen_y > 256) {
        return;
    }

    dl(rdram, 0xE7000000, 0);                      // pipe sync
    dl(rdram, 0xFD100000, 0x803A2960);             // the status palette
    dl(rdram, 0xE8000000, 0);                      // tile sync
    dl(rdram, 0xF5000100, 0x07000000);             // tile 7 at TMEM 0x800
    dl(rdram, 0xE6000000, 0);                      // load sync
    dl(rdram, 0xF0000000, 0x073FC000);             // load 256 colours
    dl(rdram, 0xE7000000, 0);                      // pipe sync
    dl(rdram, 0xBA000E02, 0x00008000);             // texture LUT: RGBA16
    draw_head(rdram, ctx, screen_x, screen_y);
}
