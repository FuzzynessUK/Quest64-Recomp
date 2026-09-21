#ifndef __ZELDA_RENDER_H__
#define __ZELDA_RENDER_H__

#include <unordered_set>
#include <filesystem>

#include "common/rt64_user_configuration.h"
#include "ultramodern/renderer_context.hpp"
#include "librecomp/mods.hpp"

namespace RT64 {
    struct Application;
}

namespace zelda64 {
    namespace renderer {
        // Extends the game's full-width 2D rectangles (fades, menu backdrops)
        // to the screen edges when the aspect ratio is expanded.
        void set_widescreen_2d_enabled(bool value);
        // Where the HP/MP block and the four spirits are drawn, in frame
        // pixels (240 tall, x from the window's left edge); `custom` false
        // leaves a block where the game puts it (Enhancements > HUD).
        void set_hud_layout(bool hp_custom, float hp_x, float hp_y, bool sp_custom, float sp_x, float sp_y);
        // Widens the game's inset scissor and frame clear to the full frame
        // so the scene reaches the window's edges (Layout: Remove black borders).
        void set_borders_removed(bool value);
        // Writes the next frame's display list to widescreen_frame.txt (F9).
        void request_widescreen_frame_dump();

        inline const std::string special_option_texture_pack_enabled = "_recomp_texture_pack_enabled";

        class RT64Context final : public ultramodern::renderer::RendererContext {
        public:
            ~RT64Context() override;
            RT64Context(uint8_t *rdram, ultramodern::renderer::WindowHandle window_handle, bool developer_mode);

            bool valid() override { return static_cast<bool>(app); }

            bool update_config(const ultramodern::renderer::GraphicsConfig &old_config, const ultramodern::renderer::GraphicsConfig &new_config) override;

            void enable_instant_present() override;
            void send_dl(const OSTask *task) override;
            void update_screen() override;
            void shutdown() override;
            uint32_t get_display_framerate() const override;
            float get_resolution_scale() const override;

        private:
            std::unique_ptr<RT64::Application> app;
            std::unordered_set<std::string> enabled_texture_packs;
            std::unordered_set<std::string> secondary_disabled_texture_packs;

            void check_texture_pack_actions();
        };

        std::unique_ptr<ultramodern::renderer::RendererContext> create_render_context(uint8_t *rdram, ultramodern::renderer::WindowHandle window_handle, bool developer_mode);

        RT64::UserConfiguration::Antialiasing RT64MaxMSAA();
        bool RT64SamplePositionsSupported();
        bool RT64HighPrecisionFBEnabled();

        void trigger_texture_pack_update();
        void enable_texture_pack(const recomp::mods::ModContext& context, const recomp::mods::ModHandle& mod);
        void disable_texture_pack(const recomp::mods::ModHandle& mod);
        void secondary_enable_texture_pack(const std::string& mod_id);
        void secondary_disable_texture_pack(const std::string& mod_id);

        // Texture pack enable option. Must be an enum with two options.
        // The first option is treated as disabled and the second option is treated as enabled.
        bool is_texture_pack_enable_config_option(const recomp::mods::ConfigOption& option, bool show_errors);
    }
}

#endif
