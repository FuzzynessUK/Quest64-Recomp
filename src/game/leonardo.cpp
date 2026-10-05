#include <algorithm>
#include <array>
#include <cstring>
#include <vector>
#include <cstdint>
#include <fstream>
#include <span>
#include <string>

#include "enhancements.h"
#include "leonardo.h"
#include "zelda_config.h"
#include "librecomp/game.hpp"
#include "recomp.h"

// Play as Leonardo (Enhancements, Quality of Life): Brian is drawn as
// Leonardo, the blond knight of Normoon and Brannoch Castle, cape and all.
//
// How a character is drawn. func_8001DB38(flags, actor, record, mtx) takes
// a 12-byte animation record { u16 bones, u16 frames, u32 bone array, u32
// mesh table } and draws every root bone, recursing through children
// (func_8001DC78). A bone is 0x20 bytes: +0 keyframes (0x34 each: three
// translation, three rotation, three scale floats, spline parameters, frame),
// +4 u16 key count, +6 u16 frames (each bone loops on its own), +8 pivot x,
// y, z, +0x14 parent index + 1 (0 = root), +0x15 mesh-table index (-1
// none), +0x16 flags, +0x18 optional list of mesh indices cycled by frame
// (Brian's face) with its length at +0x1C.
//
// Brian's records are at 0x80206064 (one per animation, all sharing the mesh
// table at 0x80206000); his bones for the current animation are streamed to
// 0x80200000 when it changes (func_80006720). He has 23 bones: head, torso,
// hips, two hair pieces, arms, legs, a staff, and a seven-bone cape.
// Leonardo is an NPC: model 0x8021DFB4 in the Normoon NPC file (ROM
// 0x95F730-0x98A7E0, which every map of that region loads to 0x8020E6F0;
// the Brannoch Castle file has him too). He has 19 bones: Brian's head,
// torso, hips, arms, legs and hand item at the same pivots, plus four pieces
// on the torso (shoulders, cape) and one on the head.
//
// Pointing Brian at Leonardo's model, as a GameShark code can, leaves both
// problems the code has: the model is in the map's NPC file, which the next
// region's file overwrites (the crash after leaving Normoon), and Leonardo's
// meshes hung on Brian's bones put his cape where Brian's is, which it is
// not. So instead: the NPC file is copied once into RAM nothing else uses
// (0x80700000, below the widescreen ring at 0x807E0000) with its pointers
// moved, and each time Brian is drawn a 19-bone Leonardo skeleton is built:
// the bones Brian has take Brian's keyframes and pivots, so Leonardo moves
// exactly as Brian does, and the five Leonardo-only bones keep Leonardo's
// own keyframes and pivots from his first animation, so his cape hangs
// where it hangs on him. The meshes are Leonardo's throughout.
namespace {
    constexpr uint32_t file_rom = 0x95F730;
    constexpr uint32_t file_rom_end = 0x98A7E0;
    constexpr int32_t file_ram = 0x8020E6F0;
    constexpr int32_t file_size = static_cast<int32_t>(file_rom_end - file_rom);
    constexpr int32_t copy_ram = 0x80700000;
    constexpr int32_t leo_model = 0x8021DFB4;     // his record table, as the game loads it
    constexpr int leo_bones = 19;
    constexpr int32_t synth_record = copy_ram + file_size + 0x100;
    constexpr int32_t synth_bones = synth_record + 0x10;
    constexpr int32_t brian = 0x8007BACC;

    // For each of Leonardo's bones, the bone of Brian's that drives it, or -1
    // for one of Leonardo's own (the torso and head pieces).
    constexpr std::array<int, leo_bones> brian_bone = {
        0,  1,  2,          // head, torso, hips
        5,  6,              // right upper arm, forearm
        10, 11,             // left upper arm, forearm
        7,  8,  9,          // right thigh, shin, foot
        12, 13, 14,         // left thigh, shin, foot
        -1, -1, -1, -1,     // on the torso: shoulders and cape
        21,                 // the item in the left hand (Brian's staff)
        -1,                 // on the head
    };

    // The other way round, for the Leonardo NPCs while the player is
    // Leonardo: Brian's 23 bones and meshes (the mesh table at 0x80206000 is
    // resident), the bones Leonardo has driven by the NPC's own keyframes,
    // the rest (hair, cape, staff, the blinking face) by Brian's idle
    // animation, copied out of its stream (ROM 0xF14A10-0xF15FE0, loaded to
    // 0x80200000 when it plays, bones at 0x802012E4) so it is there
    // whatever Brian is doing.
    constexpr uint32_t idle_rom = 0xF14A10;
    constexpr uint32_t idle_rom_end = 0xF15FE0;
    constexpr int32_t idle_ram = 0x80200000;
    constexpr int32_t idle_bones_at = 0x802012E4;
    constexpr int32_t idle_copy = synth_bones + 0x400;
    constexpr int32_t idle_size = static_cast<int32_t>(idle_rom_end - idle_rom);
    constexpr int brian_count_idle = 23;
    constexpr int32_t npc_record = idle_copy + idle_size + 0x100;
    constexpr int32_t npc_bones = npc_record + 0x10;
    constexpr int32_t brian_mesh_table = 0x80206000;
    bool idle_prepared = false;
    bool idle_failed = false;

    // The files that carry Leonardo (Normoon's and Brannoch Castle's), with
    // his mesh table's address as the game loads each: an NPC is Leonardo
    // when its record has 19 bones and that mesh table, and the table's first
    // words are the file's own.
    struct LeoFile { uint32_t rom; int32_t mesh; };
    constexpr LeoFile leo_files[] = { { 0x95F730, 0x8021DF68 }, { 0x9E3930, 0x8021F1B8 } };

    bool prepared = false;
    bool failed = false;
    int32_t leo_record = 0;     // his first animation record, in the copy

    void log_line(const std::string& line) {
        std::ofstream out(zelda64::get_app_folder_path() / "leonardo.txt", std::ios::app);
        out << line << "\n";
    }

    bool in_file(uint32_t addr) {
        return addr >= static_cast<uint32_t>(file_ram) && addr < static_cast<uint32_t>(file_ram + file_size);
    }
    int32_t moved(uint32_t addr) {
        return static_cast<int32_t>(addr - static_cast<uint32_t>(file_ram) + static_cast<uint32_t>(copy_ram));
    }

    // The NPC file into the copy, then every pointer the drawing follows
    // moved with it: the mesh table, the vertex (0x04) and texture image
    // (0xFD) addresses in each mesh's display list, and his bones' keyframe
    // pointers. Leonardo's display lists call nothing else. A word already
    // moved is outside the file's range, so a list two meshes share is only
    // moved once.
    bool prepare(uint8_t* rdram) {
        std::span<const uint8_t> rom = recomp::get_rom();
        if (rom.size() < file_rom_end) {
            log_line("ROM too small; not prepared");
            return false;
        }
        for (int32_t i = 0; i < file_size; i++) {
            MEM_B(i, copy_ram) = static_cast<int8_t>(rom[file_rom + i]);
        }
        leo_record = moved(leo_model);
        int count = MEM_HU(0, leo_record);
        uint32_t bones = static_cast<uint32_t>(MEM_W(4, leo_record));
        uint32_t mesh = static_cast<uint32_t>(MEM_W(8, leo_record));
        if (count != leo_bones || !in_file(bones) || !in_file(mesh)) {
            log_line("Leonardo's record is not where it should be (" + std::to_string(count) + " bones); not prepared");
            return false;
        }
        MEM_W(4, leo_record) = moved(bones);
        MEM_W(8, leo_record) = moved(mesh);
        const int32_t table = moved(mesh);
        int moved_words = 0;
        for (int i = 0; i < leo_bones; i++) {
            uint32_t dl = static_cast<uint32_t>(MEM_W(i * 4, table));
            if (in_file(dl)) {
                dl = static_cast<uint32_t>(moved(dl));
                MEM_W(i * 4, table) = static_cast<int32_t>(dl);
            }
            if ((dl >> 24) != 0x80) {
                continue;
            }
            for (int c = 0; c < 4000; c++) {
                int32_t at = static_cast<int32_t>(dl) + c * 8;
                uint32_t w0 = static_cast<uint32_t>(MEM_W(0, at));
                uint32_t w1 = static_cast<uint32_t>(MEM_W(4, at));
                uint32_t op = w0 >> 24;
                if ((op == 0x04 || op == 0xFD) && in_file(w1)) {
                    MEM_W(4, at) = moved(w1);
                    moved_words++;
                }
                if (op == 0xB8) {
                    break;
                }
            }
        }
        const int32_t bone_array = moved(bones);
        for (int i = 0; i < leo_bones; i++) {
            uint32_t keys = static_cast<uint32_t>(MEM_W(i * 0x20, bone_array));
            if (in_file(keys)) {
                MEM_W(i * 0x20, bone_array) = moved(keys);
            }
        }
        log_line("prepared: " + std::to_string(moved_words) + " display-list addresses moved");
        return true;
    }

    void copy_bone(uint8_t* rdram, int32_t from, int32_t to) {
        for (int i = 0; i < 0x20; i += 4) {
            MEM_W(i, to) = MEM_W(i, from);
        }
    }
}

namespace {
    bool is_leonardo_npc(uint8_t* rdram, int32_t record) {
        if (MEM_HU(0, record) != leo_bones) {
            return false;
        }
        const int32_t mesh = MEM_W(8, record);
        std::span<const uint8_t> rom = recomp::get_rom();
        for (const LeoFile& f : leo_files) {
            if (mesh != f.mesh) {
                continue;
            }
            const uint32_t at = f.rom + static_cast<uint32_t>(mesh - file_ram);
            if (rom.size() < at + 8) {
                return false;
            }
            auto word = [&](uint32_t o) {
                return static_cast<int32_t>((rom[o] << 24) | (rom[o + 1] << 16) | (rom[o + 2] << 8) | rom[o + 3]);
            };
            return MEM_W(0, mesh) == word(at) && MEM_W(4, mesh) == word(at + 4);
        }
        return false;
    }

    bool prepare_idle(uint8_t* rdram) {
        std::span<const uint8_t> rom = recomp::get_rom();
        if (rom.size() < idle_rom_end) {
            return false;
        }
        for (int32_t i = 0; i < idle_size; i++) {
            MEM_B(i, idle_copy) = static_cast<int8_t>(rom[idle_rom + i]);
        }
        const int32_t bones = idle_copy + (idle_bones_at - idle_ram);
        for (int i = 0; i < brian_count_idle; i++) {
            uint32_t keys = static_cast<uint32_t>(MEM_W(i * 0x20, bones));
            if (keys < static_cast<uint32_t>(idle_ram) || keys >= static_cast<uint32_t>(idle_ram + idle_size)) {
                log_line("Brian's idle animation is not laid out as expected; the NPC stays Leonardo");
                return false;
            }
            MEM_W(i * 0x20, bones) = static_cast<int32_t>(keys - static_cast<uint32_t>(idle_ram) + static_cast<uint32_t>(idle_copy));
        }
        return true;
    }

    // A Leonardo NPC drawn as Brian.
    void npc_as_brian(uint8_t* rdram, recomp_context* ctx) {
        if (idle_failed) {
            return;
        }
        if (!idle_prepared) {
            idle_prepared = true;
            idle_failed = !prepare_idle(rdram);
            if (idle_failed) {
                return;
            }
        }
        const int32_t record = static_cast<int32_t>(ctx->r6);
        const int32_t leo = MEM_W(4, record);
        const int32_t idle = idle_copy + (idle_bones_at - idle_ram);
        for (int b = 0; b < brian_count_idle; b++) {
            const int32_t out = npc_bones + b * 0x20;
            copy_bone(rdram, idle + b * 0x20, out);
            int from = -1;
            for (int l = 0; l < leo_bones; l++) {
                if (brian_bone[l] == b) {
                    from = l;
                    break;
                }
            }
            if (from >= 0) {
                // The NPC's keyframes and pivot; Brian's place in the tree,
                // his mesh and (on the head) his face list.
                const int32_t src = leo + from * 0x20;
                for (int i = 0; i < 0x14; i += 4) {
                    MEM_W(i, out) = MEM_W(i, src);
                }
                MEM_B(0x16, out) = MEM_B(0x16, src);
            }
        }
        MEM_H(0, npc_record) = brian_count_idle;
        MEM_H(2, npc_record) = MEM_H(2, record);
        MEM_W(4, npc_record) = npc_bones;
        MEM_W(8, npc_record) = brian_mesh_table;
        ctx->r6 = npc_record;
    }
}

// func_8001DB38 at its first instruction: a1 the actor, a2 the animation
// record. For Brian, the record is swapped for Leonardo's skeleton driven by
// Brian's current animation; for a Leonardo NPC, for Brian's skeleton driven
// by the NPC's.
extern "C" void quest64_leonardo_draw(uint8_t* rdram, recomp_context* ctx) {
    if (!zelda64::enhancements::active_options().play_as_leonardo) {
        return;
    }
    if (static_cast<int32_t>(ctx->r5) != brian) {
        if (is_leonardo_npc(rdram, static_cast<int32_t>(ctx->r6))) {
            npc_as_brian(rdram, ctx);
        }
        return;
    }
    if (failed) {
        return;
    }
    if (!prepared) {
        prepared = true;
        failed = !prepare(rdram);
        if (failed) {
            return;
        }
    }
    const int32_t record = static_cast<int32_t>(ctx->r6);
    const int brian_count = MEM_HU(0, record);
    const int32_t brian_bones = MEM_W(4, record);
    const int32_t leo_bone_array = MEM_W(4, leo_record);
    if (brian_count <= 21 || (static_cast<uint32_t>(brian_bones) >> 24) != 0x80) {
        return;   // not an animation this was made for: Brian as he is
    }
    for (int i = 0; i < leo_bones; i++) {
        const int32_t leo = leo_bone_array + i * 0x20;
        const int32_t out = synth_bones + i * 0x20;
        const int from = brian_bone[i];
        if (from >= 0 && from < brian_count) {
            // Brian's keyframes, pivot and flags; Leonardo's place in the
            // tree and his mesh.
            copy_bone(rdram, brian_bones + from * 0x20, out);
            MEM_B(0x14, out) = MEM_B(0x14, leo);
            MEM_B(0x15, out) = MEM_B(0x15, leo);
            MEM_W(0x18, out) = 0;
            MEM_W(0x1C, out) = 0;
        }
        else {
            copy_bone(rdram, leo, out);
        }
    }
    MEM_H(0, synth_record) = leo_bones;
    MEM_H(2, synth_record) = MEM_H(2, record);
    MEM_W(4, synth_record) = synth_bones;
    MEM_W(8, synth_record) = MEM_W(8, leo_record);
    ctx->r6 = synth_record;
}

// The name. The NPC files and the messages are both read from the ROM as
// they are needed (a map's NPC file as it loads, a message each time it is
// shown), so the ROM image is what is changed. Text here is the game's
// message encoding: 0x81 capitals, 0x82 lower case, letters from 0.
namespace {
    const std::vector<uint8_t> leonardo_text = { 0x81, 0x0B, 0x82, 0x04, 0x0E, 0x0D, 0x00, 0x11, 0x03, 0x0E };
    const std::vector<uint8_t> brian_text = { 0x81, 0x01, 0x82, 0x11, 0x08, 0x00, 0x0D };

    // Replaces the name at `at` if it is there, moving the rest of the text
    // (up to its FF end) down to close the gap and putting the spare bytes
    // after the end, so nothing reads them.
    bool rename_at(uint8_t* rom, size_t size, uint32_t at) {
        if (at + leonardo_text.size() >= size ||
            !std::equal(leonardo_text.begin(), leonardo_text.end(), rom + at)) {
            return false;
        }
        size_t end = at + leonardo_text.size();
        while (end < size && rom[end] != 0xFF && end - at < 0x800) {
            end++;
        }
        if (end >= size || rom[end] != 0xFF) {
            return false;
        }
        const size_t gap = leonardo_text.size() - brian_text.size();
        std::copy(brian_text.begin(), brian_text.end(), rom + at);
        std::memmove(rom + at + brian_text.size(), rom + at + leonardo_text.size(), end + 1 - (at + leonardo_text.size()));
        std::fill(rom + end + 1 - gap, rom + end + 1, 0xFF);
        return true;
    }
}

void zelda64::leonardo::apply_at_boot() {
    if (!zelda64::enhancements::active_options().play_as_leonardo) {
        return;
    }
    std::span<const uint8_t> rom = recomp::get_rom();
    uint8_t* data = const_cast<uint8_t*>(rom.data());
    // His name line in the Normoon and Brannoch Castle NPC files, and "I'm
    // Leonardo" in the message he greets Brian with.
    constexpr uint32_t places[] = { 0x98A7A6, 0xA163D6, 0xD36B87 };
    int done = 0;
    for (uint32_t at : places) {
        done += rename_at(data, rom.size(), at) ? 1 : 0;
    }
    log_line("named Brian in " + std::to_string(done) + " of 3 places");
}
