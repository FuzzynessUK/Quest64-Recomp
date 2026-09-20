#include <atomic>
#include <cstring>
#include <span>
#include <string>
#include <vector>

#include "hardmode.h"
#include "enhancements.h"
#include "hardmode_rup.h"
#include "librecomp/game.hpp"
#include "recomp.h"
#include "ultramodern/save.hpp"

namespace {
    std::atomic<bool> applied = false;

    // Where the hack's boot code puts the appended payload.
    constexpr uint32_t payload_rom_start = 0x1000000;
    constexpr int32_t payload_ram_start = 0x80400000;

    // The boot segment was copied to RAM before on_init ran, so writes that
    // land in it have to be mirrored (see randomizer::apply_at_boot).
    constexpr uint32_t boot_rom_start = 0x1000;
    constexpr uint32_t boot_size = 0x100000;
    constexpr int32_t boot_ram_start = 0x80000400;

    // A parsed Ninja 2 patch. Format: romhacking.net document 288.
    struct RupPatch {
        std::string version;
        uint32_t source_size = 0;
        uint32_t target_size = 0;
        // 'A' appends overflow (XORed with 0xFF) when the target is bigger.
        char overflow_mode = 0;
        std::span<const uint8_t> overflow;
        struct Record {
            uint32_t offset;
            std::span<const uint8_t> xor_bytes;
        };
        std::vector<Record> records;
        bool ok = false;
    };

    RupPatch parse_rup(std::span<const uint8_t> p) {
        RupPatch patch;
        if (p.size() < 0x800 || std::memcmp(p.data(), "NINJA2", 6) != 0) {
            return patch;
        }
        // Header: magic(6) encoding(1) author(84) version(11) ...
        const char* version = reinterpret_cast<const char*>(p.data() + 6 + 1 + 84);
        patch.version.assign(version, strnlen(version, 11));

        size_t pos = 0x800;
        auto vlv = [&](uint32_t& out) {
            if (pos >= p.size()) return false;
            uint8_t n = p[pos++];
            if (n > 4 || pos + n > p.size()) return false;
            out = 0;
            for (int i = 0; i < n; i++) {
                out |= static_cast<uint32_t>(p[pos++]) << (8 * i);
            }
            return true;
        };
        while (pos < p.size()) {
            uint8_t cmd = p[pos++];
            if (cmd == 0x00) {
                patch.ok = true;
                return patch;
            }
            if (cmd == 0x01) {
                uint32_t name_len;
                if (!vlv(name_len)) return patch;
                pos += name_len + 1; // name, ROM type
                if (!vlv(patch.source_size) || !vlv(patch.target_size)) return patch;
                pos += 32; // source and target MD5
                if (patch.source_size != patch.target_size) {
                    if (pos >= p.size()) return patch;
                    patch.overflow_mode = static_cast<char>(p[pos++]);
                    uint32_t len;
                    if (!vlv(len) || pos + len > p.size()) return patch;
                    patch.overflow = p.subspan(pos, len);
                    pos += len;
                }
            }
            else if (cmd == 0x02) {
                uint32_t offset, len;
                if (!vlv(offset) || !vlv(len) || pos + len > p.size()) return patch;
                patch.records.push_back({ offset, p.subspan(pos, len) });
                pos += len;
            }
            else {
                return patch;
            }
        }
        return patch;
    }

    std::span<const uint8_t> embedded_patch() {
        return { reinterpret_cast<const uint8_t*>(hardmode_rup), hardmode_rup_size };
    }
}

bool zelda64::hardmode::active() {
    return applied.load(std::memory_order_relaxed);
}

std::string zelda64::hardmode::patch_version() {
    return parse_rup(embedded_patch()).version;
}

void zelda64::hardmode::apply_at_boot(uint8_t* rdram) {
    if (!zelda64::enhancements::active_options().hard_mode) {
        return;
    }
    RupPatch patch = parse_rup(embedded_patch());
    if (!patch.ok) {
        return;
    }

    std::span<const uint8_t> rom = recomp::get_rom();
    if (patch.source_size != 0 && rom.size() != patch.source_size) {
        return;
    }
    std::vector<uint8_t> patched(rom.begin(), rom.end());
    if (patch.overflow_mode == 'A') {
        patched.reserve(patched.size() + patch.overflow.size());
        for (uint8_t b : patch.overflow) {
            patched.push_back(static_cast<uint8_t>(b ^ 0xFF));
        }
    }
    else if (patch.overflow_mode == 'M') {
        patched.resize(patch.target_size);
    }

    for (const RupPatch::Record& record : patch.records) {
        for (size_t i = 0; i < record.xor_bytes.size(); i++) {
            uint32_t offset = record.offset + static_cast<uint32_t>(i);
            if (offset >= patched.size()) {
                break;
            }
            // Records past the original size XOR against zero, i.e. they
            // are the bytes themselves.
            uint8_t source = offset < rom.size() ? rom[offset] : 0;
            patched[offset] = static_cast<uint8_t>(source ^ record.xor_bytes[i]);
            if (offset >= boot_rom_start && offset < boot_rom_start + boot_size) {
                MEM_B(0, boot_ram_start + static_cast<int32_t>(offset - boot_rom_start)) =
                    static_cast<int8_t>(patched[offset]);
            }
        }
    }

    // What the hack's boot hook DMAs: everything appended to the ROM, to
    // 0x80400000. The console needs the Expansion Pak for this; the recomp
    // always has 8 MB.
    for (size_t offset = payload_rom_start; offset < patched.size(); offset++) {
        MEM_B(0, payload_ram_start + static_cast<int32_t>(offset - payload_rom_start)) =
            static_cast<int8_t>(patched[offset]);
    }

    recomp::set_rom_contents(std::move(patched));
    applied.store(true, std::memory_order_relaxed);
}

void zelda64::hardmode::on_frame(uint8_t* rdram) {
    static bool save_folder_set = false;
    if (save_folder_set || !active()) {
        return;
    }
    save_folder_set = true;
    // The saving thread is started after on_init, so this cannot happen at
    // boot; the first frame is still long before the game touches the
    // Controller Pak.
    ultramodern::change_save_file(u8"hardmode", recomp::current_game_id());
}
