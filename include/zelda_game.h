#ifndef __ZELDA_GAME_H__
#define __ZELDA_GAME_H__

#include <cstdint>
#include <span>
#include <vector>

namespace zelda64 {
    // Relaunch the application, which comes back up on the boot menu. There
    // is no in-place reset: ultramodern can start a game but has no way to
    // tear a running one down, so an actual soft reset would need changes in
    // the N64ModernRuntime submodule.
    void restart_application();

    void quicksave_save();
    void quicksave_load();
    std::vector<uint8_t> decompress_sf64(std::span<const uint8_t> compressed_rom);
};

#endif
