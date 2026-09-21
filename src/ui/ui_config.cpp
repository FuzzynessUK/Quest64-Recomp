#include <algorithm>
#include <chrono>
#include <cmath>
#include <random>
#include <type_traits>

#include "recomp_ui.h"
#include "recomp_input.h"
#include "zelda_sound.h"
#include "zelda_config.h"
#include "zelda_debug.h"
#include "zelda_game.h"
#include "randomizer.h"
#include "audio.h"
#include "enhancements.h"
#include "statfx.h"
#include "speedrun.h"
#include "zelda_render.h"
#include "zelda_support.h"
#include "promptfont.h"
#include "ultramodern/config.hpp"
#include "ultramodern/ultramodern.hpp"
#include "RmlUi/Core.h"

#include "core/ui_context.h"

ultramodern::renderer::GraphicsConfig new_options;
Rml::DataModelHandle nav_help_model_handle;
Rml::DataModelHandle general_model_handle;
Rml::DataModelHandle controls_model_handle;
Rml::DataModelHandle graphics_model_handle;
Rml::DataModelHandle sound_options_model_handle;

// True if controller config menu is open, false if keyboard config menu is open, undefined otherwise
bool configuring_controller = false;

int recompui::config_tab_to_index(recompui::ConfigTab tab) {
    switch (tab) {
    case recompui::ConfigTab::General:
        return 0;
    case recompui::ConfigTab::Controls:
        return 1;
    case recompui::ConfigTab::Graphics:
        return 2;
    case recompui::ConfigTab::Sound:
        return 3;
    case recompui::ConfigTab::Mods:
        return 4;
    case recompui::ConfigTab::Cheats:
        return 5;
    case recompui::ConfigTab::Randomizer:
        return 6;
    case recompui::ConfigTab::Enhancements:
        return 7;
    case recompui::ConfigTab::Audio:
        return 8;
    case recompui::ConfigTab::Debug:
        return 9;
    default:
        assert(false && "Unknown config tab.");
        return 0;
    }
}

template <typename T>
void get_option(const T& input, Rml::Variant& output) {
    std::string value = "";
    to_json(value, input);

    if (value.empty()) {
        throw std::runtime_error("Invalid value :" + std::to_string(int(input)));
    }

    output = value;
}

template <typename T>
void set_option(T& output, const Rml::Variant& input) {
    T value = T::OptionCount;
    from_json(input.Get<std::string>(), value);

    if (value == T::OptionCount) {
        throw std::runtime_error("Invalid value :" + input.Get<std::string>());
    }

    output = value;
}

template <typename T>
void bind_option(Rml::DataModelConstructor& constructor, const std::string& name, T* option) {
    constructor.BindFunc(name,
        [option](Rml::Variant& out) { get_option(*option, out); },
        [option](const Rml::Variant& in) {
            set_option(*option, in);
            graphics_model_handle.DirtyVariable("options_changed");
            graphics_model_handle.DirtyVariable("ds_info");
        }
    );
};

template <typename T>
void bind_atomic(Rml::DataModelConstructor& constructor, Rml::DataModelHandle handle, const char* name, std::atomic<T>* atomic_val) {
    constructor.BindFunc(name,
        [atomic_val](Rml::Variant& out) {
            out = atomic_val->load();
        },
        [atomic_val, handle, name](const Rml::Variant& in) mutable {
            atomic_val->store(in.Get<T>());
            handle.DirtyVariable(name);
        }
    );
}

static int scanned_binding_index = -1;
static int scanned_input_index = -1;
static int focused_input_index = -1;
static int focused_config_option_index = -1;

static bool msaa2x_supported = false;
static bool msaa4x_supported = false;
static bool msaa8x_supported = false;
static bool sample_positions_supported = false;

static bool cont_active = true;

static recomp::InputDevice cur_device = recomp::InputDevice::Controller;

int recomp::get_scanned_input_index() {
    return scanned_input_index;
}

void recomp::finish_scanning_input(recomp::InputField scanned_field) {
    recomp::set_input_binding(static_cast<recomp::GameInput>(scanned_input_index), scanned_binding_index, cur_device, scanned_field);
    scanned_input_index = -1;
    scanned_binding_index = -1;
    controls_model_handle.DirtyVariable("inputs");
    controls_model_handle.DirtyVariable("active_binding_input");
    controls_model_handle.DirtyVariable("active_binding_slot");
    nav_help_model_handle.DirtyVariable("nav_help__accept");
    nav_help_model_handle.DirtyVariable("nav_help__exit");
    graphics_model_handle.DirtyVariable("gfx_help__apply");
}

void recomp::cancel_scanning_input() {
    recomp::stop_scanning_input();
    scanned_input_index = -1;
    scanned_binding_index = -1;
    controls_model_handle.DirtyVariable("inputs");
    controls_model_handle.DirtyVariable("active_binding_input");
    controls_model_handle.DirtyVariable("active_binding_slot");
    nav_help_model_handle.DirtyVariable("nav_help__accept");
    nav_help_model_handle.DirtyVariable("nav_help__exit");
    graphics_model_handle.DirtyVariable("gfx_help__apply");
}

void recomp::config_menu_set_cont_or_kb(bool cont_interacted) {
    if (cont_active != cont_interacted) {
        cont_active = cont_interacted;

        if (nav_help_model_handle) {
            nav_help_model_handle.DirtyVariable("nav_help__navigate");
            nav_help_model_handle.DirtyVariable("nav_help__accept");
            nav_help_model_handle.DirtyVariable("nav_help__exit");
        }

        if (graphics_model_handle) {
            graphics_model_handle.DirtyVariable("gfx_help__apply");
        }
    }
}

void close_config_menu_impl() {
    zelda64::save_config();

    recompui::ContextId config_context = recompui::get_config_context_id();
    recompui::ContextId sub_menu_context = recompui::get_config_sub_menu_context_id();

    if (recompui::is_context_shown(sub_menu_context)) {
    	recompui::hide_context(sub_menu_context);
    }
    else {
    	recompui::hide_context(config_context);
    }

    if (!ultramodern::is_game_started()) {
        recompui::show_context(recompui::get_launcher_context_id(), "");
    }
}

// TODO: Remove once RT64 gets native fullscreen support on Linux
#if defined(__linux__)
extern SDL_Window* window;
#endif

void apply_graphics_config(void) {
    ultramodern::renderer::set_graphics_config(new_options);
#if defined(__linux__) // TODO: Remove once RT64 gets native fullscreen support on Linux
    if (new_options.wm_option == ultramodern::renderer::WindowMode::Fullscreen) {
        SDL_SetWindowFullscreen(window,SDL_WINDOW_FULLSCREEN_DESKTOP);
    } else {
        SDL_SetWindowFullscreen(window,0);
    }
#endif
}

void close_config_menu() {
    if (ultramodern::renderer::get_graphics_config() != new_options) {
        recompui::open_choice_prompt(
            "Graphics options have changed",
            "Would you like to apply or discard the changes?",
            "Apply",
            "Discard",
            []() {
                apply_graphics_config();
                graphics_model_handle.DirtyAllVariables();
                close_config_menu_impl();
            },
            []() {
                new_options = ultramodern::renderer::get_graphics_config();
                graphics_model_handle.DirtyAllVariables();
                close_config_menu_impl();
            },
            recompui::ButtonVariant::Success,
            recompui::ButtonVariant::Error,
            true,
            "config__close-menu-button"
        );
        return;
    }

    close_config_menu_impl();
}

void zelda64::open_quit_game_prompt() {
    recompui::open_choice_prompt(
        "Are you sure you want to quit?",
        "Any progress since your last save will be lost.",
        "Quit",
        "Cancel",
        []() {
            ultramodern::quit();
        },
        []() {},
        recompui::ButtonVariant::Error,
        recompui::ButtonVariant::Tertiary,
        true,
        "config__quit-game-button"
    );
}

// These defaults values don't matter, as the config file handling overrides them.
struct ControlOptionsContext {
    int rumble_strength; // 0 to 100
    int gyro_sensitivity; // 0 to 100
    int mouse_sensitivity; // 0 to 100
    int joystick_deadzone; // 0 to 100
    zelda64::TargetingMode targeting_mode;
    recomp::BackgroundInputMode background_input_mode;
    zelda64::FilmGrainMode film_grain_mode;
    zelda64::Widescreen2DMode widescreen_2d_mode = zelda64::Widescreen2DMode::On;
    zelda64::RadioBoxMode radio_comm_box_mode;
    zelda64::AimInvertMode invert_y_axis_mode;
    zelda64::AimInvertMode analog_camera_invert_mode;
};

ControlOptionsContext control_options_context;

int recomp::get_rumble_strength() {
    return control_options_context.rumble_strength;
}

void recomp::set_rumble_strength(int strength) {
    control_options_context.rumble_strength = strength;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("rumble_strength");
    }
}

int recomp::get_gyro_sensitivity() {
    return control_options_context.gyro_sensitivity;
}

int recomp::get_mouse_sensitivity() {
    return control_options_context.mouse_sensitivity;
}

int recomp::get_joystick_deadzone() {
    return control_options_context.joystick_deadzone;
}

void recomp::set_gyro_sensitivity(int sensitivity) {
    control_options_context.gyro_sensitivity = sensitivity;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("gyro_sensitivity");
    }
}

void recomp::set_mouse_sensitivity(int sensitivity) {
    control_options_context.mouse_sensitivity = sensitivity;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("mouse_sensitivity");
    }
}

void recomp::set_joystick_deadzone(int deadzone) {
    control_options_context.joystick_deadzone = deadzone;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("joystick_deadzone");
    }
}

zelda64::TargetingMode zelda64::get_targeting_mode() {
    return control_options_context.targeting_mode;
}

void zelda64::set_targeting_mode(zelda64::TargetingMode mode) {
    control_options_context.targeting_mode = mode;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("targeting_mode");
    }
}

recomp::BackgroundInputMode recomp::get_background_input_mode() {
    return control_options_context.background_input_mode;
}

void recomp::set_background_input_mode(recomp::BackgroundInputMode mode) {
    control_options_context.background_input_mode = mode;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("background_input_mode");
    }
    SDL_SetHint(
        SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,
        mode == recomp::BackgroundInputMode::On
            ? "1"
            : "0"
    );
}

zelda64::FilmGrainMode zelda64::get_film_grain_mode() {
    return control_options_context.film_grain_mode;
}

void zelda64::set_film_grain_mode(zelda64::FilmGrainMode mode) {
    control_options_context.film_grain_mode = mode;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("film_grain_mode");
    }
}

zelda64::Widescreen2DMode zelda64::get_widescreen_2d_mode() {
    return control_options_context.widescreen_2d_mode;
}

void zelda64::set_widescreen_2d_mode(zelda64::Widescreen2DMode mode) {
    control_options_context.widescreen_2d_mode = mode;
    zelda64::renderer::set_widescreen_2d_enabled(mode == zelda64::Widescreen2DMode::On);
    if (general_model_handle) {
        general_model_handle.DirtyVariable("widescreen_2d_mode");
    }
}

zelda64::RadioBoxMode zelda64::get_radio_comm_box_mode() {
    return control_options_context.radio_comm_box_mode;
}

void zelda64::set_radio_comm_box_mode(zelda64::RadioBoxMode mode) {
    control_options_context.radio_comm_box_mode = mode;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("radio_comm_box_mode");
    }
}

zelda64::AimInvertMode zelda64::get_invert_y_axis_mode() {
    return control_options_context.invert_y_axis_mode;
}

void zelda64::set_invert_y_axis_mode(zelda64::AimInvertMode mode) {
    control_options_context.invert_y_axis_mode = mode;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("invert_y_axis_mode");
    }
}

zelda64::AimInvertMode zelda64::get_analog_camera_invert_mode() {
    return control_options_context.analog_camera_invert_mode;
}

void zelda64::set_analog_camera_invert_mode(zelda64::AimInvertMode mode) {
    control_options_context.analog_camera_invert_mode = mode;
    if (general_model_handle) {
        general_model_handle.DirtyVariable("analog_camera_invert_mode");
    }
}

struct SoundOptionsContext {
    std::atomic<int> main_volume; // Option to control the volume of all sound
    std::atomic<int> bgm_volume;
    std::atomic<int> sfx_volume;
	std::atomic<int> voice_volume;
    std::atomic<int> low_health_beeps_enabled; // RmlUi doesn't seem to like "true"/"false" strings for setting variants so an int is used here instead.
    void reset() {
        bgm_volume = 100;
        sfx_volume = 100;
		voice_volume = 100;
        main_volume = 100;
        low_health_beeps_enabled = (int)true;
    }
    SoundOptionsContext() {
        reset();
    }
};

SoundOptionsContext sound_options_context;

void zelda64::reset_sound_settings() {
    sound_options_context.reset();
    if (sound_options_model_handle) {
        sound_options_model_handle.DirtyAllVariables();
    }
}

void zelda64::set_main_volume(int volume) {
    sound_options_context.main_volume.store(volume);
    if (sound_options_model_handle) {
        sound_options_model_handle.DirtyVariable("main_volume");
    }
}

int zelda64::get_main_volume() {
    return sound_options_context.main_volume.load();
}

void zelda64::set_bgm_volume(int volume) {
    sound_options_context.bgm_volume.store(volume);
    if (sound_options_model_handle) {
        sound_options_model_handle.DirtyVariable("bgm_volume");
    }
}

int zelda64::get_bgm_volume() {
    return sound_options_context.bgm_volume.load();
}

void zelda64::set_sfx_volume(int volume) {
    sound_options_context.sfx_volume.store(volume);
	if (sound_options_model_handle) {
		sound_options_model_handle.DirtyVariable("sfx_volume");
	}
}

int zelda64::get_sfx_volume() {
    return sound_options_context.sfx_volume.load();
}

void zelda64::set_voice_volume(int volume) {
    sound_options_context.voice_volume.store(volume);
	if (sound_options_model_handle) {
		sound_options_model_handle.DirtyVariable("voice_volume");
	}
}

int zelda64::get_voice_volume() {
    return sound_options_context.voice_volume.load();
}

void zelda64::set_low_health_beeps_enabled(bool enabled) {
    sound_options_context.low_health_beeps_enabled.store((int)enabled);
    if (sound_options_model_handle) {
        sound_options_model_handle.DirtyVariable("low_health_beeps_enabled");
    }
}

bool zelda64::get_low_health_beeps_enabled() {
    return (bool)sound_options_context.low_health_beeps_enabled.load();
}

struct DebugContext {
    Rml::DataModelHandle model_handle;
    std::vector<std::string> area_names;
    std::vector<std::string> scene_names;
    std::vector<std::string> entrance_names; 
    int area_index = 0;
    int scene_index = 0;
    int entrance_index = 0;
    int set_time_day = 1;
    int set_time_hour = 12;
    int set_time_minute = 0;
    bool debug_enabled = false;

    DebugContext() {
        for (const auto& area : zelda64::game_warps) {
            area_names.emplace_back(area.name);
        }
        update_warp_names();
    }

    void update_warp_names() {
        scene_names.clear();
        for (const auto& scene : zelda64::game_warps[area_index].scenes) {
            scene_names.emplace_back(scene.name);
        }
        
        entrance_names = zelda64::game_warps[area_index].scenes[scene_index].entrances;
    }
};

DebugContext debug_context;

struct CheatsContext {
    Rml::DataModelHandle model_handle;

    int warp_map = 13;
    int warp_submap = 17;
    int warp_entrance = 0;
    // Dropdown labels, indexed by id. The map list is fixed; the other two
    // are rebuilt for the selected map and submap.
    std::vector<std::string> warp_map_names;
    std::vector<std::string> warp_submap_names;
    std::vector<std::string> warp_entrance_names;

    // Get Item list, and the master switch for the whole tab.
    int item_index = 0;
    std::vector<std::string> item_names;
    bool all_cheats_enabled = true;

    // What the sliders show. Follows the game's value except right after the
    // user moves a slider, when it holds the new value until the game reports
    // it back.
    int shown_stats[static_cast<size_t>(zelda64::PlayerStat::Count)] = {};
    // The game's value as of the last refresh, so only changes get dirtied.
    int game_stats[static_cast<size_t>(zelda64::PlayerStat::Count)] = {};
    bool shown_stats_available = false;
};

CheatsContext cheats_context;

void init_warp_map_names() {
    for (int map = 0; map < zelda64::map_count(); map++) {
        cheats_context.warp_map_names.emplace_back(std::to_string(map) + ": " + zelda64::map_name(map));
    }
}

constexpr std::pair<const char*, zelda64::PlayerStat> cheat_stat_names[] = {
    { "cheat_stat_hp", zelda64::PlayerStat::HP },
    { "cheat_stat_max_hp", zelda64::PlayerStat::MaxHP },
    { "cheat_stat_mp", zelda64::PlayerStat::MP },
    { "cheat_stat_max_mp", zelda64::PlayerStat::MaxMP },
    { "cheat_stat_agility", zelda64::PlayerStat::Agility },
    { "cheat_stat_defense", zelda64::PlayerStat::Defense },
    { "cheat_stat_fire", zelda64::PlayerStat::Fire },
    { "cheat_stat_earth", zelda64::PlayerStat::Earth },
    { "cheat_stat_wind", zelda64::PlayerStat::Wind },
    { "cheat_stat_water", zelda64::PlayerStat::Water },
};

// Warp map/submap/entrance sliders. Each map has a different number of
// submaps and each submap a different number of entrances, so choosing a
// map re-ranges the sliders below it and pulls their values into range.
void clamp_warp_selection() {
    int& map = cheats_context.warp_map;
    int& submap = cheats_context.warp_submap;
    int& entrance = cheats_context.warp_entrance;
    map = std::clamp(map, 0, zelda64::map_count() - 1);
    submap = std::clamp(submap, 0, zelda64::submap_count(map) - 1);
    entrance = std::clamp(entrance, 0, zelda64::entrance_count(map, submap) - 1);

    cheats_context.warp_submap_names.clear();
    for (int i = 0; i < zelda64::submap_count(map); i++) {
        cheats_context.warp_submap_names.push_back(zelda64::submap_label(map, i));
    }
    cheats_context.warp_entrance_names.clear();
    for (int i = 0; i < zelda64::entrance_count(map, submap); i++) {
        cheats_context.warp_entrance_names.push_back(zelda64::entrance_label(map, submap, i));
    }
}

void dirty_warp_selection() {
    for (const char* name : { "cheat_warp_map", "cheat_warp_submap", "cheat_warp_submap_names", "cheat_warp_entrance", "cheat_warp_entrance_names" }) {
        cheats_context.model_handle.DirtyVariable(name);
    }
}

void bind_warp_selection(Rml::DataModelConstructor& constructor) {
    auto bind_field = [&constructor](const char* name, int CheatsContext::* field) {
        constructor.BindFunc(name,
            [field](Rml::Variant& out) { out = cheats_context.*field; },
            [field](const Rml::Variant& in) {
                cheats_context.*field = in.Get<int>();
                clamp_warp_selection();
                dirty_warp_selection();
            }
        );
    };
    bind_field("cheat_warp_map", &CheatsContext::warp_map);
    bind_field("cheat_warp_submap", &CheatsContext::warp_submap);
    bind_field("cheat_warp_entrance", &CheatsContext::warp_entrance);

    clamp_warp_selection();
    constructor.Bind("cheat_warp_submap_names", &cheats_context.warp_submap_names);
    constructor.Bind("cheat_warp_entrance_names", &cheats_context.warp_entrance_names);
}

// Randomizer tab. Every control writes straight back to the options file, so
// what the file says is always what the menu shows; the game reads the file
// once at boot.
struct RandomizerContext {
    Rml::DataModelHandle model_handle;
    zelda64::randomizer::Options edited;
    // The saved presets, which one is selected in the list, and which one
    // was last loaded or saved. preset_naming is true while the Save button
    // has the name box open.
    std::vector<std::string> preset_list;
    int preset_index = 0;
    std::string current_preset;
    std::string preset_name;
    bool preset_naming = false;
    std::string preset_message;
    // Set once anything on the tab is changed; the "applies on next
    // launch" line is shown while it is.
    bool changed = false;
};

RandomizerContext randomizer_context;

// Keeps the preset list in sync with what is on disk.
void refresh_preset_list() {
    randomizer_context.preset_list = zelda64::randomizer::preset_names();
    int last = static_cast<int>(randomizer_context.preset_list.size()) - 1;
    randomizer_context.preset_index = last < 0 ? 0 : std::clamp(randomizer_context.preset_index, 0, last);
}

std::string selected_preset() {
    const std::vector<std::string>& list = randomizer_context.preset_list;
    if (list.empty()) {
        return std::string();
    }
    int last = static_cast<int>(list.size()) - 1;
    return list[static_cast<size_t>(std::clamp(randomizer_context.preset_index, 0, last))];
}

// What the last Save/Load/Delete did, shown under the preset controls.
std::string randomizer_presets_status() {
    if (randomizer_context.preset_message.empty()) {
        return "Presets keep every setting except the seed.";
    }
    return randomizer_context.preset_message;
}

void randomizer_option_changed() {
    zelda64::randomizer::save_options(randomizer_context.edited);
    randomizer_context.changed = true;
    randomizer_context.model_handle.DirtyVariable("rnd_changed");
}

// Hover tooltips for the debug-style tabs. Each option label carries its
// text in a hidden child (.config-tip-text); on mouseover it is copied into
// the tab's one floating .config-tip, which lives outside the scroll
// container so the scroll clip never cuts it off. The tip hangs below the
// label, or above it when the label is in the lower half of the panel.
Rml::Element* find_tab_ancestor(Rml::Element* element) {
    for (Rml::Element* e = element; e != nullptr; e = e->GetParentNode()) {
        if (e->IsClassSet("config-debug")) {
            return e;
        }
    }
    return nullptr;
}

Rml::Element* find_child_by_class(Rml::Element* parent, const char* class_name) {
    if (parent == nullptr) {
        return nullptr;
    }
    for (int i = 0; i < parent->GetNumChildren(); i++) {
        Rml::Element* child = parent->GetChild(i);
        if (child->IsClassSet(class_name)) {
            return child;
        }
    }
    return nullptr;
}

void bind_tooltip_events(Rml::DataModelConstructor& constructor) {
    constructor.BindEventCallback("show_tip",
        [](Rml::DataModelHandle, Rml::Event& event, const Rml::VariantList&) {
            Rml::Element* label = event.GetCurrentElement();
            Rml::Element* tab = find_tab_ancestor(label);
            Rml::Element* text = find_child_by_class(label, "config-tip-text");
            Rml::Element* tip = find_child_by_class(tab, "config-tip");
            if (text == nullptr || tip == nullptr) {
                return;
            }
            tip->SetInnerRML(text->GetInnerRML());

            Rml::Vector2f tab_origin = tab->GetAbsoluteOffset(Rml::BoxArea::Padding);
            Rml::Vector2f tab_size = tab->GetBox().GetSize(Rml::BoxArea::Padding);
            Rml::Vector2f label_origin = label->GetAbsoluteOffset(Rml::BoxArea::Border) - tab_origin;
            Rml::Vector2f label_size = label->GetBox().GetSize(Rml::BoxArea::Border);

            tip->SetProperty(Rml::PropertyId::Left, Rml::Property(label_origin.x, Rml::Unit::PX));
            if (label_origin.y + label_size.y / 2.0f < tab_size.y / 2.0f) {
                tip->RemoveProperty("bottom");
                tip->SetProperty(Rml::PropertyId::Top, Rml::Property(label_origin.y + label_size.y, Rml::Unit::PX));
            }
            else {
                tip->RemoveProperty("top");
                tip->SetProperty(Rml::PropertyId::Bottom, Rml::Property(tab_size.y - label_origin.y, Rml::Unit::PX));
            }
            tip->SetProperty("display", "block");
        });
    constructor.BindEventCallback("hide_tip",
        [](Rml::DataModelHandle, Rml::Event& event, const Rml::VariantList&) {
            // mouseout bubbles up from the label's children while the pointer
            // is still on the label; only the label's own counts.
            if (event.GetCurrentElement() != event.GetTargetElement()) {
                return;
            }
            Rml::Element* tip = find_child_by_class(find_tab_ancestor(event.GetCurrentElement()), "config-tip");
            if (tip != nullptr) {
                tip->SetProperty("display", "none");
            }
        });
}

template <typename T>
void bind_randomizer_field(Rml::DataModelConstructor& constructor, const char* name, T zelda64::randomizer::Options::* field) {
    constructor.BindFunc(name,
        [field](Rml::Variant& out) {
            if constexpr (std::is_same_v<T, std::string>) {
                out = randomizer_context.edited.*field;
            }
            else {
                out = static_cast<int>(randomizer_context.edited.*field);
            }
        },
        [field](const Rml::Variant& in) {
            if constexpr (std::is_same_v<T, std::string>) {
                randomizer_context.edited.*field = in.Get<std::string>();
            }
            else {
                randomizer_context.edited.*field = static_cast<T>(in.Get<int>());
            }
            randomizer_option_changed();
        }
    );
}


// Enhancements tab. Same shape as the randomizer tab: every control writes
// straight back to its own settings file, which the game reads once at boot.
struct EnhancementsContext {
    Rml::DataModelHandle model_handle;
    zelda64::enhancements::Options edited;
    // Set once an option that only applies at boot is changed; the
    // "applies on next launch" line is shown while it is.
    bool changed = false;
};

EnhancementsContext enhancements_context;

void enhancements_option_changed(bool needs_relaunch = true) {
    zelda64::enhancements::save_options(enhancements_context.edited);
    if (needs_relaunch) {
        enhancements_context.changed = true;
    }
    if (enhancements_context.model_handle) {
        enhancements_context.model_handle.DirtyVariable("enh_changed");
    }
}

// Hard Mode is switched from the Mods tab but lives in enhancements.json,
// so the mod menu goes through the same edited copy the tab saves.
bool recompui::is_hard_mode_enabled() {
    return enhancements_context.edited.hard_mode;
}

void recompui::set_hard_mode_enabled(bool enabled) {
    enhancements_context.edited.hard_mode = enabled;
    enhancements_option_changed();
}


// N64 mode, in the spirit of Ship of Harkinian's: render the way the console
// did. Applied straight to the graphics config rather than the ROM, and the
// previous settings are kept in the enhancements file so switching it off puts
// them back instead of falling to defaults.
void apply_n64_mode(bool on) {
    using namespace ultramodern::renderer;
    zelda64::enhancements::Options& o = enhancements_context.edited;

    if (on) {
        o.saved_resolution = static_cast<int>(new_options.res_option);
        o.saved_aspect = static_cast<int>(new_options.ar_option);
        o.saved_antialiasing = static_cast<int>(new_options.msaa_option);
        o.saved_hud_ratio = static_cast<int>(new_options.hr_option);

        new_options.res_option = Resolution::Original;
        new_options.ar_option = AspectRatio::Original;
        new_options.msaa_option = Antialiasing::None;
        new_options.hr_option = HUDRatioMode::Original;
    }
    else {
        auto restore = [](int saved, auto& field, auto fallback) {
            using T = std::decay_t<decltype(field)>;
            field = (saved >= 0 && saved < static_cast<int>(T::OptionCount))
                ? static_cast<T>(saved) : fallback;
        };
        restore(o.saved_resolution, new_options.res_option, Resolution::Auto);
        restore(o.saved_aspect, new_options.ar_option, AspectRatio::Expand);
        restore(o.saved_antialiasing, new_options.msaa_option, Antialiasing::None);
        restore(o.saved_hud_ratio, new_options.hr_option, HUDRatioMode::Clamp16x9);
    }

    apply_graphics_config();
}

void make_enhancements_bindings(Rml::Context* context) {
    Rml::DataModelConstructor constructor = context->CreateDataModel("enhancements_model");
    if (!constructor) {
        throw std::runtime_error("Failed to make RmlUi data model for the enhancements menu");
    }

    enhancements_context.edited = zelda64::enhancements::load_options();

    constructor.BindFunc("enh_changed", [](Rml::Variant& out) { out = enhancements_context.changed ? 1 : 0; });
    bind_tooltip_events(constructor);
    constructor.BindFunc("enh_one_hit_ko",
        [](Rml::Variant& out) { out = enhancements_context.edited.one_hit_ko ? 1 : 0; },
        [](const Rml::Variant& in) {
            enhancements_context.edited.one_hit_ko = in.Get<int>() != 0;
            enhancements_option_changed();
        }
    );

    constructor.BindFunc("enh_exit_from_anywhere",
        [](Rml::Variant& out) { out = enhancements_context.edited.exit_from_anywhere ? 1 : 0; },
        [](const Rml::Variant& in) {
            enhancements_context.edited.exit_from_anywhere = in.Get<int>() != 0;
            enhancements_option_changed();
        }
    );

    constructor.BindFunc("enh_faster_walk",
        [](Rml::Variant& out) { out = enhancements_context.edited.faster_walk ? 1 : 0; },
        [](const Rml::Variant& in) {
            enhancements_context.edited.faster_walk = in.Get<int>() != 0;
            enhancements_option_changed();
        }
    );
    constructor.BindFunc("enh_stat_up_effect",
        [](Rml::Variant& out) { out = enhancements_context.edited.stat_up_effect ? 1 : 0; },
        [](const Rml::Variant& in) {
            enhancements_context.edited.stat_up_effect = in.Get<int>() != 0;
            enhancements_option_changed();
        }
    );
    constructor.BindFunc("enh_jp_healing",
        [](Rml::Variant& out) { out = enhancements_context.edited.jp_healing ? 1 : 0; },
        [](const Rml::Variant& in) {
            enhancements_context.edited.jp_healing = in.Get<int>() != 0;
            enhancements_option_changed();
        }
    );

    constructor.BindFunc("enh_longer_magic_barrier",
        [](Rml::Variant& out) { out = enhancements_context.edited.longer_magic_barrier ? 1 : 0; },
        [](const Rml::Variant& in) {
            enhancements_context.edited.longer_magic_barrier = in.Get<int>() != 0;
            enhancements_option_changed();
        }
    );

    constructor.BindFunc("enh_n64_mode",
        [](Rml::Variant& out) { out = enhancements_context.edited.n64_mode ? 1 : 0; },
        [](const Rml::Variant& in) {
            bool on = in.Get<int>() != 0;
            if (on != enhancements_context.edited.n64_mode) {
                apply_n64_mode(on);
                enhancements_context.edited.n64_mode = on;
                enhancements_option_changed(false);
            }
        }
    );

    // One control: 0 = off, 1..6 = on at timer_position 0..5. The two
    // settings stay separate in enhancements.json.
    constructor.BindFunc("enh_speedrun_timer",
        [](Rml::Variant& out) {
            const auto& e = enhancements_context.edited;
            out = e.speedrun_timer ? e.timer_position + 1 : 0;
        },
        [](const Rml::Variant& in) {
            int value = std::clamp(in.Get<int>(), 0, 6);
            enhancements_context.edited.speedrun_timer = value != 0;
            if (value != 0) {
                enhancements_context.edited.timer_position = value - 1;
            }
            enhancements_option_changed();
        }
    );

    enhancements_context.model_handle = constructor.GetModelHandle();
}

// Audio tab. Same shape as the enhancements tab: one settings file, read
// once at boot, with the "next launch" line shown once something changed.
struct AudioContext {
    Rml::DataModelHandle model_handle;
    zelda64::audio::Options edited;
    bool changed = false;
};

AudioContext audio_context;

void audio_option_changed() {
    zelda64::audio::save_options(audio_context.edited);
    audio_context.changed = true;
    audio_context.model_handle.DirtyVariable("aud_changed");
}

void make_audio_bindings(Rml::Context* context) {
    Rml::DataModelConstructor constructor = context->CreateDataModel("audio_model");
    if (!constructor) {
        throw std::runtime_error("Failed to make RmlUi data model for the audio menu");
    }

    audio_context.edited = zelda64::audio::load_options();

    constructor.BindFunc("aud_changed", [](Rml::Variant& out) { out = audio_context.changed ? 1 : 0; });
    bind_tooltip_events(constructor);
    constructor.BindFunc("aud_music_shuffle",
        [](Rml::Variant& out) { out = static_cast<int>(audio_context.edited.music_shuffle); },
        [](const Rml::Variant& in) {
            audio_context.edited.music_shuffle = static_cast<zelda64::audio::MusicShuffle>(std::clamp(in.Get<int>(), 0, 2));
            audio_option_changed();
        }
    );
    constructor.BindFunc("aud_sfx_shuffle",
        [](Rml::Variant& out) { out = audio_context.edited.sfx_shuffle ? 1 : 0; },
        [](const Rml::Variant& in) {
            audio_context.edited.sfx_shuffle = in.Get<int>() != 0;
            audio_option_changed();
        }
    );

    audio_context.model_handle = constructor.GetModelHandle();
}

void make_randomizer_bindings(Rml::Context* context) {
    using zelda64::randomizer::Options;
    Rml::DataModelConstructor constructor = context->CreateDataModel("randomizer_model");
    if (!constructor) {
        throw std::runtime_error("Failed to make RmlUi data model for the randomizer menu");
    }

    randomizer_context.edited = zelda64::randomizer::load_options();

    constructor.BindFunc("rnd_changed", [](Rml::Variant& out) { out = randomizer_context.changed ? 1 : 0; });
    bind_tooltip_events(constructor);
    constructor.BindFunc("rnd_presets", [](Rml::Variant& out) { out = randomizer_presets_status(); });
    constructor.RegisterArray<std::vector<std::string>>();
    refresh_preset_list();
    constructor.Bind("rnd_preset_list", &randomizer_context.preset_list);
    constructor.Bind("rnd_preset_index", &randomizer_context.preset_index);
    constructor.BindFunc("rnd_current_preset", [](Rml::Variant& out) {
        out = randomizer_context.current_preset.empty()
            ? std::string("Current preset: none")
            : "Current preset: " + randomizer_context.current_preset;
    });
    constructor.BindFunc("rnd_preset_naming", [](Rml::Variant& out) { out = randomizer_context.preset_naming ? 1 : 0; });
    constructor.BindFunc("rnd_preset_name",
        [](Rml::Variant& out) { out = randomizer_context.preset_name; },
        [](const Rml::Variant& in) { randomizer_context.preset_name = in.Get<std::string>(); }
    );
    bind_randomizer_field(constructor, "rnd_mode", &Options::mode);
    bind_randomizer_field(constructor, "rnd_seed", &Options::seed);
    bind_randomizer_field(constructor, "rnd_spell_shuffle", &Options::spell_shuffle);
    bind_randomizer_field(constructor, "rnd_hinted_spell_names", &Options::hinted_spell_names);
    bind_randomizer_field(constructor, "rnd_linear_spell_names", &Options::linear_spell_names);
    bind_randomizer_field(constructor, "rnd_early_healing", &Options::early_healing);
    bind_randomizer_field(constructor, "rnd_extra_healing", &Options::extra_healing);
    bind_randomizer_field(constructor, "rnd_distribute_spells", &Options::distribute_spells);
    bind_randomizer_field(constructor, "rnd_spell_overrides", &Options::spell_overrides);
    bind_randomizer_field(constructor, "rnd_spell_rebalance", &Options::spell_rebalance);
    bind_randomizer_field(constructor, "rnd_bubble", &Options::bubble);
    bind_randomizer_field(constructor, "rnd_level_1_spells", &Options::level_1_spells);
    bind_randomizer_field(constructor, "rnd_max_accuracy", &Options::max_accuracy);
    bind_randomizer_field(constructor, "rnd_max_accuracy_all", &Options::max_accuracy_all);
    bind_randomizer_field(constructor, "rnd_soul_search", &Options::soul_search);
    bind_randomizer_field(constructor, "rnd_invalidity", &Options::invalidity);
    bind_randomizer_field(constructor, "rnd_chests", &Options::chests);
    bind_randomizer_field(constructor, "rnd_drops", &Options::drops);
    bind_randomizer_field(constructor, "rnd_gifts", &Options::gifts);
    bind_randomizer_field(constructor, "rnd_wingsmiths", &Options::wingsmiths);
    bind_randomizer_field(constructor, "rnd_shuffle_shannon", &Options::shuffle_shannon);
    bind_randomizer_field(constructor, "rnd_wingsmith_wings_only", &Options::wingsmith_wings_only);
    bind_randomizer_field(constructor, "rnd_boss_rewards_shuffler", &Options::boss_rewards_shuffler);
    bind_randomizer_field(constructor, "rnd_monster_stats", &Options::monster_stats);
    bind_randomizer_field(constructor, "rnd_variance", &Options::variance);
    bind_randomizer_field(constructor, "rnd_monster_scale", &Options::monster_scale);
    bind_randomizer_field(constructor, "rnd_scale_percent", &Options::scale_percent);
    bind_randomizer_field(constructor, "rnd_exp_by_bst", &Options::exp_by_bst);
    bind_randomizer_field(constructor, "rnd_exp_boost", &Options::exp_boost);
    bind_randomizer_field(constructor, "rnd_boss_order", &Options::boss_order);
    bind_randomizer_field(constructor, "rnd_boss_element", &Options::boss_element);
    bind_randomizer_field(constructor, "rnd_start_hp", &Options::start_hp);
    bind_randomizer_field(constructor, "rnd_start_mp", &Options::start_mp);
    bind_randomizer_field(constructor, "rnd_start_agility", &Options::start_agility);
    bind_randomizer_field(constructor, "rnd_start_defense", &Options::start_defense);
    bind_randomizer_field(constructor, "rnd_fast_monastery", &Options::fast_monastery);
    bind_randomizer_field(constructor, "rnd_fast_blue_cave", &Options::fast_blue_cave);
    bind_randomizer_field(constructor, "rnd_fast_shamwood", &Options::fast_shamwood);
    bind_randomizer_field(constructor, "rnd_fast_mammon", &Options::fast_mammon);
    bind_randomizer_field(constructor, "rnd_unlock_doors", &Options::unlock_doors);
    bind_randomizer_field(constructor, "rnd_locked_endgame", &Options::locked_endgame);
    bind_randomizer_field(constructor, "rnd_crystal_return", &Options::crystal_return);
    bind_randomizer_field(constructor, "rnd_brannoch_return", &Options::brannoch_return);
    bind_randomizer_field(constructor, "rnd_mammon_door", &Options::mammon_door);
    bind_randomizer_field(constructor, "rnd_restless_npcs", &Options::restless_npcs);
    bind_randomizer_field(constructor, "rnd_max_message_speed", &Options::max_message_speed);
    bind_randomizer_field(constructor, "rnd_hud_lock", &Options::hud_lock);
    bind_randomizer_field(constructor, "rnd_celtland_drift", &Options::celtland_drift);
    bind_randomizer_field(constructor, "rnd_level_2_spells", &Options::level_2_spells);
    bind_randomizer_field(constructor, "rnd_reveal_spirits", &Options::reveal_spirits);
    bind_randomizer_field(constructor, "rnd_better_dew_drop", &Options::better_dew_drop);
    bind_randomizer_field(constructor, "rnd_zoom_out", &Options::zoom_out);
    bind_randomizer_field(constructor, "rnd_ivory_wings", &Options::ivory_wings);
    bind_randomizer_field(constructor, "rnd_text_improvements", &Options::text_improvements);
    bind_randomizer_field(constructor, "rnd_lost_keys", &Options::lost_keys);
    bind_randomizer_field(constructor, "rnd_fire_book", &Options::fire_book);
    bind_randomizer_field(constructor, "rnd_shannon_hints", &Options::shannon_hints);
    bind_randomizer_field(constructor, "rnd_enemy_tables", &Options::enemy_tables);
    bind_randomizer_field(constructor, "rnd_enemy_randomizer", &Options::enemy_randomizer);
    bind_randomizer_field(constructor, "rnd_encounter_rate", &Options::encounter_rate);
    bind_randomizer_field(constructor, "rnd_mp_regain", &Options::mp_regain);
    bind_randomizer_field(constructor, "rnd_staff_hit_mp", &Options::staff_hit_mp);
    bind_randomizer_field(constructor, "rnd_element_uncap", &Options::element_uncap);
    bind_randomizer_field(constructor, "rnd_drop_limit_disabled", &Options::drop_limit_disabled);
    bind_randomizer_field(constructor, "rnd_wing_unlock_indoors", &Options::wing_unlock_indoors);
    bind_randomizer_field(constructor, "rnd_wing_unlock_skye", &Options::wing_unlock_skye);
    bind_randomizer_field(constructor, "rnd_text_palette", &Options::text_palette);
    bind_randomizer_field(constructor, "rnd_staff_palette", &Options::staff_palette);
    bind_randomizer_field(constructor, "rnd_cloak_palette", &Options::cloak_palette);
    bind_randomizer_field(constructor, "rnd_brian_palette", &Options::brian_palette);
    bind_randomizer_field(constructor, "rnd_spell_palette", &Options::spell_palette);

    randomizer_context.model_handle = constructor.GetModelHandle();
}

// The slider shows the value the game last reported and a change is queued
// straight back to the game, so there is no separate apply step.
void bind_player_stat(Rml::DataModelConstructor& constructor, const char* name, zelda64::PlayerStat stat) {
    constructor.BindFunc(name,
        [stat](Rml::Variant& out) {
            out = cheats_context.shown_stats[static_cast<size_t>(stat)];
        },
        [stat](const Rml::Variant& in) {
            // Sliders fire a change while the document loads (a min="1" slider
            // clamps its initial 0), long before the game has values. Only
            // treat a change as the user's once the game has reported its
            // stats, and only if it is actually different.
            int value = in.Get<int>();
            if (!cheats_context.shown_stats_available || value == cheats_context.game_stats[static_cast<size_t>(stat)]) {
                return;
            }
            cheats_context.shown_stats[static_cast<size_t>(stat)] = value;
            zelda64::set_player_stat(stat, value);
        }
    );
}

// Pull the game's latest stat values into the cheats model. Runs on the UI
// thread each frame the menu is open.
void recompui::update_cheats_model() {
    if (!cheats_context.model_handle) {
        return;
    }

    bool available = zelda64::player_stats_available();
    if (available != cheats_context.shown_stats_available) {
        cheats_context.shown_stats_available = available;
        cheats_context.model_handle.DirtyVariable("cheat_stats_available");
    }
    if (!available) {
        return;
    }

    for (const auto& [name, stat] : cheat_stat_names) {
        int value = zelda64::get_player_stat(stat);
        int& game_value = cheats_context.game_stats[static_cast<size_t>(stat)];
        if (value != game_value) {
            game_value = value;
            cheats_context.shown_stats[static_cast<size_t>(stat)] = value;
            cheats_context.model_handle.DirtyVariable(name);
        }
    }
}


// Speedrun timer overlay. Its own context so it can stay up while the game is
// running; it never captures input or the mouse, so it only draws.
struct SpeedrunContext {
    Rml::DataModelHandle model_handle;
    std::string shown_text;
    bool shown_visible = false;
    bool shown_finished = false;
};

SpeedrunContext speedrun_context_state;
recompui::ContextId speedrun_context;

recompui::ContextId recompui::get_speedrun_context_id() {
    return speedrun_context;
}

void recompui::show_speedrun_overlay() {
    if (speedrun_context == recompui::ContextId::null()) {
        return;
    }
    if (!recompui::is_context_shown(speedrun_context)) {
        recompui::show_context(speedrun_context, "");
    }
}

// Refreshes the clock. Runs on the UI thread every frame, so it only marks
// variables dirty when the text it would draw has actually changed.
void recompui::update_speedrun_model() {
    if (!speedrun_context_state.model_handle) {
        return;
    }

    // Nothing before the game is actually running, so the boot menu stays
    // clean. Put the overlay back if anything hid it, such as a menu closing.
    bool wanted = zelda64::enhancements::active_options().speedrun_timer
        && ultramodern::is_game_started();
    if (wanted) {
        recompui::show_speedrun_overlay();
    }

    // Reads 00:00:00 until the title hands over, so it is obvious the
    // overlay is working before a run is under way.
    bool visible = wanted;
    if (visible != speedrun_context_state.shown_visible) {
        speedrun_context_state.shown_visible = visible;
        speedrun_context_state.model_handle.DirtyVariable("timer_visible");
    }
    if (!visible) {
        return;
    }

    bool done = zelda64::speedrun::finished();
    if (done != speedrun_context_state.shown_finished) {
        speedrun_context_state.shown_finished = done;
        speedrun_context_state.model_handle.DirtyVariable("timer_finished");
    }

    std::string text = zelda64::speedrun::display();
    if (text != speedrun_context_state.shown_text) {
        speedrun_context_state.shown_text = std::move(text);
        speedrun_context_state.model_handle.DirtyVariable("timer_text");
    }
}

void make_speedrun_bindings(Rml::Context* context) {
    Rml::DataModelConstructor constructor = context->CreateDataModel("speedrun_model");
    if (!constructor) {
        throw std::runtime_error("Failed to make RmlUi data model for the speedrun timer");
    }

    constructor.BindFunc("timer_text",
        [](Rml::Variant& out) { out = speedrun_context_state.shown_text; });
    constructor.BindFunc("timer_visible",
        [](Rml::Variant& out) { out = speedrun_context_state.shown_visible; });
    constructor.BindFunc("timer_finished",
        [](Rml::Variant& out) { out = speedrun_context_state.shown_finished; });
    constructor.BindFunc("timer_position",
        [](Rml::Variant& out) { out = zelda64::enhancements::active_options().timer_position; });

    speedrun_context_state.model_handle = constructor.GetModelHandle();
}

// JP stat-up effect overlay. Like the timer, its own draw-only context that
// stays up while the game runs. The game thread reports rises and Brian's
// screen position (statfx.cpp); this side lays a soft halo over him in the
// stat's colour, swelling in and fading out, moved every frame so it stays
// on him if the camera moves. The halo is one texture generated here (the
// renderer only takes images handed to it in memory) and tinted with
// image-color per stat.
namespace {
    struct Glow {
        Rml::Element* element;
        float age;
    };

    recompui::ContextId statfx_context;
    std::vector<Glow> glows;
    std::chrono::steady_clock::time_point statfx_last_update{};
    // The last place Brian was seen, kept while he is off screen so a halo
    // in progress does not jump.
    float statfx_feet_px[2] = { 0.0f, 0.0f };
    float statfx_head_px[2] = { 0.0f, 0.0f };
    bool statfx_seen = false;

    constexpr const char* glow_texture = "?/statfx/glow";
    constexpr int glow_texture_size = 128;

    // Seconds: fade in while expanding, hold, then shrink while fading out.
    constexpr float glow_in = 0.3f;
    constexpr float glow_hold = 0.15f;
    constexpr float glow_out = 0.5f;
    constexpr float glow_life = glow_in + glow_hold + glow_out;
    constexpr float glow_peak_opacity = 1.0f;
    // The halo's diameter as a multiple of Brian's on-screen height: where
    // it starts, its full size, and what it shrinks to before it is gone.
    constexpr float glow_scale_start = 1.6f;
    constexpr float glow_scale_full = 3.4f;
    constexpr float glow_scale_end = 2.4f;

    const char* glow_class(zelda64::statfx::Stat stat) {
        switch (stat) {
        case zelda64::statfx::Stat::HP: return "glow--hp";
        case zelda64::statfx::Stat::MP: return "glow--mp";
        case zelda64::statfx::Stat::Defense: return "glow--defense";
        default: return "glow--agility";
        }
    }

    // White: a small bright core, then a thin haze that fades to the edge,
    // so the colour covers him but he stays visible through it.
    void make_glow_texture() {
        std::vector<char> bytes(glow_texture_size * glow_texture_size * 4);
        float half = glow_texture_size / 2.0f;
        for (int y = 0; y < glow_texture_size; y++) {
            for (int x = 0; x < glow_texture_size; x++) {
                float dx = (x + 0.5f - half) / half;
                float dy = (y + 0.5f - half) / half;
                float r = std::sqrt(dx * dx + dy * dy);
                constexpr float core = 0.08f;       // bright centre
                constexpr float core_blend = 0.10f; // softening past it
                constexpr float haze = 0.4f;        // the rest, at most
                float t = std::min(std::max((r - core) / (1.0f - core), 0.0f), 1.0f);
                float a = haze * (1.0f - t) * (1.0f - t) * (1.0f + 2.0f * t);
                if (r < core + core_blend) {
                    float b = std::max((r - core) / core_blend, 0.0f);
                    a = 1.0f + (a - 1.0f) * b;
                }
                size_t i = (static_cast<size_t>(y) * glow_texture_size + x) * 4;
                bytes[i + 0] = static_cast<char>(255);
                bytes[i + 1] = static_cast<char>(255);
                bytes[i + 2] = static_cast<char>(255);
                bytes[i + 3] = static_cast<char>(std::lround(a * 255.0f));
            }
        }
        recompui::queue_image_from_bytes_rgba32(glow_texture, bytes, glow_texture_size, glow_texture_size);
    }

    // NDC to window pixels. The game projects at 4:3 and RT64 keeps that
    // area the full window height, centred, whatever the window's shape.
    void ndc_to_px(const Rml::Vector2i& size, float ndc_x, float ndc_y, float out[2]) {
        float height = static_cast<float>(size.y);
        out[0] = size.x / 2.0f + ndc_x * (height * 4.0f / 3.0f) / 2.0f;
        out[1] = height / 2.0f - ndc_y * height / 2.0f;
    }
}

void recompui::update_stat_effects() {
    if (statfx_context == recompui::ContextId::null()) {
        return;
    }
    bool wanted = zelda64::enhancements::active_options().stat_up_effect && ultramodern::is_game_started();
    if (!wanted) {
        return;
    }
    if (!recompui::is_context_shown(statfx_context)) {
        recompui::show_context(statfx_context, "");
    }

    Rml::ElementDocument* document = statfx_context.get_document();
    Rml::Element* container = document ? document->GetElementById("statfx") : nullptr;
    if (container == nullptr) {
        return;
    }
    Rml::Context* context = document->GetContext();
    Rml::Vector2i size = context->GetDimensions();
    float dp = context->GetDensityIndependentPixelRatio();

    auto now = std::chrono::steady_clock::now();
    float dt = statfx_last_update.time_since_epoch().count() == 0 ? 0.0f
        : std::chrono::duration<float>(now - statfx_last_update).count();
    statfx_last_update = now;
    dt = std::min(dt, 0.1f);

    zelda64::statfx::Anchor anchor = zelda64::statfx::anchor();
    if (anchor.valid) {
        ndc_to_px(size, anchor.feet_x, anchor.feet_y, statfx_feet_px);
        ndc_to_px(size, anchor.head_x, anchor.head_y, statfx_head_px);
        statfx_seen = true;
    }

    for (const zelda64::statfx::Event& event : zelda64::statfx::take_events()) {
        if (!statfx_seen) {
            // No projection yet: glow in the middle of the screen rather
            // than nowhere, so a broken projection is visible, not silent.
            statfx_feet_px[0] = statfx_head_px[0] = size.x / 2.0f;
            statfx_feet_px[1] = size.y / 2.0f + 60.0f * dp;
            statfx_head_px[1] = size.y / 2.0f - 60.0f * dp;
        }
        Rml::ElementPtr made = document->CreateElement("div");
        made->SetClass("glow", true);
        made->SetClass(glow_class(event.stat), true);
        glows.push_back({ container->AppendChild(std::move(made)), 0.0f });
    }

    // Brian's on-screen height, with a floor so a distant Brian still gets
    // a visible halo.
    float body_dx = statfx_head_px[0] - statfx_feet_px[0];
    float body_dy = statfx_head_px[1] - statfx_feet_px[1];
    float body = std::max(std::sqrt(body_dx * body_dx + body_dy * body_dy), 40.0f * dp);
    float centre_x = (statfx_feet_px[0] + statfx_head_px[0]) / 2.0f;
    float centre_y = (statfx_feet_px[1] + statfx_head_px[1]) / 2.0f;

    for (size_t i = 0; i < glows.size();) {
        Glow& glow = glows[i];
        glow.age += dt;
        if (glow.age >= glow_life) {
            container->RemoveChild(glow.element);
            glows[i] = glows.back();
            glows.pop_back();
            continue;
        }
        float opacity;
        float scale;
        if (glow.age < glow_in) {
            // Eased so it bursts out and settles.
            float f = glow.age / glow_in;
            float eased = 1.0f - (1.0f - f) * (1.0f - f);
            opacity = glow_peak_opacity * eased;
            scale = glow_scale_start + (glow_scale_full - glow_scale_start) * eased;
        }
        else if (glow.age < glow_in + glow_hold) {
            opacity = glow_peak_opacity;
            scale = glow_scale_full;
        }
        else {
            float f = (glow.age - glow_in - glow_hold) / glow_out;
            opacity = glow_peak_opacity * (1.0f - f) * (1.0f - f);
            scale = glow_scale_full + (glow_scale_end - glow_scale_full) * f;
        }
        float diameter = body * scale;
        glow.element->SetProperty(Rml::PropertyId::Left, Rml::Property(centre_x - diameter / 2.0f, Rml::Unit::PX));
        glow.element->SetProperty(Rml::PropertyId::Top, Rml::Property(centre_y - diameter / 2.0f, Rml::Unit::PX));
        glow.element->SetProperty(Rml::PropertyId::Width, Rml::Property(diameter, Rml::Unit::PX));
        glow.element->SetProperty(Rml::PropertyId::Height, Rml::Property(diameter, Rml::Unit::PX));
        glow.element->SetProperty(Rml::PropertyId::Opacity, Rml::Property(opacity, Rml::Unit::NUMBER));
        i++;
    }
}

recompui::ContextId config_context;

recompui::ContextId recompui::get_config_context_id() {
	return config_context;
}

// Helper copied from RmlUi to get a named child.
Rml::Element* recompui::get_child_by_tag(Rml::Element* parent, const std::string& tag)
{
	// Look for the existing child
	for (int i = 0; i < parent->GetNumChildren(); i++)
	{
        Rml::Element* child = parent->GetChild(i);
		if (child->GetTagName() == tag)
			return child;
	}

    return nullptr;
}

class ConfigTabsetListener : public Rml::EventListener {
    void ProcessEvent(Rml::Event& event) override {
        if (event.GetId() == Rml::EventId::Tabchange) {
            int tab_index = event.GetParameter<int>("tab_index", 0);
            bool in_mod_tab = (tab_index == recompui::config_tab_to_index(recompui::ConfigTab::Mods));
            if (in_mod_tab) {
                recompui::set_config_tabset_mod_nav();
            }
            else {
                Rml::ElementTabSet* tabset = recompui::get_config_tabset();
                Rml::Element* tabs = recompui::get_child_by_tag(tabset, "tabs");
                if (tabs != nullptr) {
                    size_t num_children = tabs->GetNumChildren();
                    for (size_t i = 0; i < num_children; i++) {
                        tabs->GetChild(i)->SetProperty(Rml::PropertyId::NavDown, Rml::Style::Nav::Auto);
                    }
                }
            }
        }
    }
};

class ConfigMenu : public recompui::MenuController {
private:
    ConfigTabsetListener config_tabset_listener;
public:
    ConfigMenu() {

    }
    ~ConfigMenu() override {

    }
    void load_document() override {
		config_context = recompui::create_context(zelda64::get_asset_path("config_menu.rml"));
        speedrun_context = recompui::create_context(zelda64::get_asset_path("speedrun_timer.rml"));
        // Draw only: the game keeps every button and the mouse.
        speedrun_context.set_captures_input(false);
        speedrun_context.set_captures_mouse(false);
        statfx_context = recompui::create_context(zelda64::get_asset_path("stat_effects.rml"));
        statfx_context.set_captures_input(false);
        statfx_context.set_captures_mouse(false);
        make_glow_texture();
        recompui::update_mod_list(false);
        recompui::get_config_tabset()->AddEventListener(Rml::EventId::Tabchange, &config_tabset_listener);
    }
    void register_events(recompui::UiEventListenerInstancer& listener) override {
        recompui::register_event(listener, "apply_options",
            [](const std::string& param, Rml::Event& event) {
                graphics_model_handle.DirtyVariable("options_changed");
                apply_graphics_config();
            });
        recompui::register_event(listener, "config_keydown",
            [](const std::string& param, Rml::Event& event) {
                if (!recompui::is_prompt_open() && event.GetId() == Rml::EventId::Keydown) {
                    auto key = event.GetParameter<Rml::Input::KeyIdentifier>("key_identifier", Rml::Input::KeyIdentifier::KI_UNKNOWN);
                    switch (key) {
                        case Rml::Input::KeyIdentifier::KI_ESCAPE:
                            close_config_menu();
                            break;
                        case Rml::Input::KeyIdentifier::KI_F:
                            graphics_model_handle.DirtyVariable("options_changed");
                            apply_graphics_config();
                            break;
                    }
                }
            });
        // This needs to be separate from `close_config_menu` so it ensures that the event is only on the target
        recompui::register_event(listener, "close_config_menu_backdrop",
            [](const std::string& param, Rml::Event& event) {
                if (event.GetPhase() == Rml::EventPhase::Target) {
                    close_config_menu();
                }
            });
        recompui::register_event(listener, "close_config_menu",
            [](const std::string& param, Rml::Event& event) {
                close_config_menu();
            });

        recompui::register_event(listener, "open_quit_game_prompt",
            [](const std::string& param, Rml::Event& event) {
                zelda64::open_quit_game_prompt();
            });

        recompui::register_event(listener, "toggle_input_device",
            [](const std::string& param, Rml::Event& event) {
                cur_device = cur_device == recomp::InputDevice::Controller
                    ? recomp::InputDevice::Keyboard
                    : recomp::InputDevice::Controller;
                controls_model_handle.DirtyVariable("input_device_is_keyboard");
                controls_model_handle.DirtyVariable("inputs");
            });
            
        recompui::register_event(listener, "area_index_changed",
            [](const std::string& param, Rml::Event& event) {
                debug_context.area_index = event.GetParameter<int>("value", 0);
                debug_context.scene_index = 0;
                debug_context.entrance_index = 0;
                debug_context.update_warp_names();
                debug_context.model_handle.DirtyVariable("scene_index");
                debug_context.model_handle.DirtyVariable("entrance_index");
                debug_context.model_handle.DirtyVariable("scene_names");
                debug_context.model_handle.DirtyVariable("entrance_names");
            });
            
        recompui::register_event(listener, "scene_index_changed",
            [](const std::string& param, Rml::Event& event) {
                debug_context.scene_index = event.GetParameter<int>("value", 0);
                debug_context.entrance_index = 0;
                debug_context.update_warp_names();
                debug_context.model_handle.DirtyVariable("entrance_index");
                debug_context.model_handle.DirtyVariable("entrance_names");
            });

        recompui::register_event(listener, "do_warp",
            [](const std::string& param, Rml::Event& event) {
                zelda64::do_warp(debug_context.area_index, debug_context.scene_index, debug_context.entrance_index);
            });

        recompui::register_event(listener, "set_time",
            [](const std::string& param, Rml::Event& event) {
                zelda64::set_time(debug_context.set_time_day, debug_context.set_time_hour, debug_context.set_time_minute);
            });

        recompui::register_event(listener, "do_map_warp",
            [](const std::string& param, Rml::Event& event) {
                zelda64::do_map_warp(cheats_context.warp_map, cheats_context.warp_submap, cheats_context.warp_entrance);
            });

        recompui::register_event(listener, "cheat_give_item",
            [](const std::string& param, Rml::Event& event) {
                zelda64::give_item(cheats_context.item_index);
            });

        recompui::register_event(listener, "reset_game",
            [](const std::string& param, Rml::Event& event) {
                zelda64::restart_application();
            });

        recompui::register_event(listener, "rnd_new_seed",
            [](const std::string& param, Rml::Event& event) {
                randomizer_context.edited.seed = std::to_string(std::random_device{}() % 100000000u);
                randomizer_option_changed();
                randomizer_context.model_handle.DirtyVariable("rnd_seed");
            });

        // Save opens the name box; Confirm is what actually writes the preset.
        recompui::register_event(listener, "rnd_save_preset",
            [](const std::string& param, Rml::Event& event) {
                randomizer_context.preset_naming = true;
                randomizer_context.preset_name = randomizer_context.current_preset;
                randomizer_context.preset_message = "Type a name, then press Confirm.";
                randomizer_context.model_handle.DirtyAllVariables();
            });

        recompui::register_event(listener, "rnd_confirm_preset",
            [](const std::string& param, Rml::Event& event) {
                const std::string name = randomizer_context.preset_name;
                if (zelda64::randomizer::save_preset(name, randomizer_context.edited)) {
                    randomizer_context.current_preset = name;
                    randomizer_context.preset_naming = false;
                    randomizer_context.preset_message = "Saved \"" + name + "\".";
                    refresh_preset_list();
                    // Select what was just saved.
                    for (size_t i = 0; i < randomizer_context.preset_list.size(); i++) {
                        if (randomizer_context.preset_list[i] == name) {
                            randomizer_context.preset_index = static_cast<int>(i);
                            break;
                        }
                    }
                }
                else {
                    randomizer_context.preset_message = "Give the preset a name first.";
                }
                randomizer_context.model_handle.DirtyAllVariables();
            });

        recompui::register_event(listener, "rnd_cancel_preset",
            [](const std::string& param, Rml::Event& event) {
                randomizer_context.preset_naming = false;
                randomizer_context.preset_message.clear();
                randomizer_context.model_handle.DirtyAllVariables();
            });

        recompui::register_event(listener, "rnd_load_preset",
            [](const std::string& param, Rml::Event& event) {
                const std::string name = selected_preset();
                if (!name.empty() && zelda64::randomizer::load_preset(name, randomizer_context.edited)) {
                    randomizer_context.current_preset = name;
                    randomizer_context.preset_message = "Loaded \"" + name + "\".";
                    randomizer_option_changed();
                }
                else {
                    randomizer_context.preset_message = "Nothing selected to load.";
                }
                // Loading can change every control on the tab.
                randomizer_context.model_handle.DirtyAllVariables();
            });

        recompui::register_event(listener, "rnd_delete_preset",
            [](const std::string& param, Rml::Event& event) {
                const std::string name = selected_preset();
                if (!name.empty() && zelda64::randomizer::delete_preset(name)) {
                    randomizer_context.preset_message = "Deleted \"" + name + "\".";
                    if (randomizer_context.current_preset == name) {
                        randomizer_context.current_preset.clear();
                    }
                    refresh_preset_list();
                }
                else {
                    randomizer_context.preset_message = "Nothing selected to delete.";
                }
                randomizer_context.model_handle.DirtyAllVariables();
            });

        // Cosmetics bulk set.
        recompui::register_event(listener, "rnd_cosmetics_default",
            [](const std::string& param, Rml::Event& event) {
                zelda64::randomizer::Options& o = randomizer_context.edited;
                o.text_palette = 0;
                o.staff_palette = false;
                o.cloak_palette = false;
                o.brian_palette = false;
                o.spell_palette = false;
                randomizer_option_changed();
                randomizer_context.model_handle.DirtyAllVariables();
            });

        recompui::register_event(listener, "rnd_cosmetics_random",
            [](const std::string& param, Rml::Event& event) {
                zelda64::randomizer::Options& o = randomizer_context.edited;
                o.text_palette = 1;
                o.staff_palette = true;
                o.cloak_palette = true;
                o.brian_palette = true;
                o.spell_palette = true;
                randomizer_option_changed();
                randomizer_context.model_handle.DirtyAllVariables();
            });
    }

    void bind_config_list_events(Rml::DataModelConstructor &constructor) {
        constructor.BindEventCallback("set_cur_config_index",
            [](Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& inputs) {
                int option_index = inputs.at(0).Get<size_t>();
                // watch for mouseout being overzealous during event bubbling, only clear if the event's attached element matches the current
                if (option_index == -1 && event.GetType() == "mouseout" && event.GetCurrentElement() != event.GetTargetElement()) {
                    return;
                }
                focused_config_option_index = option_index;
                model_handle.DirtyVariable("cur_config_index");
            });

        constructor.Bind("cur_config_index", &focused_config_option_index);
    }

    void make_graphics_bindings(Rml::Context* context) {
        Rml::DataModelConstructor constructor = context->CreateDataModel("graphics_model");
        if (!constructor) {
            throw std::runtime_error("Failed to make RmlUi data model for the graphics config menu");
        }

        ultramodern::sleep_milliseconds(50);
        new_options = ultramodern::renderer::get_graphics_config();
        bind_config_list_events(constructor);

        constructor.BindFunc("res_option",
            [](Rml::Variant& out) { get_option(new_options.res_option, out); },
            [](const Rml::Variant& in) {
                set_option(new_options.res_option, in);
                graphics_model_handle.DirtyVariable("options_changed");
                graphics_model_handle.DirtyVariable("ds_info");
                graphics_model_handle.DirtyVariable("ds_option");
            }
        );
        bind_option(constructor, "wm_option", &new_options.wm_option);
        bind_option(constructor, "ar_option", &new_options.ar_option);
        bind_option(constructor, "hr_option", &new_options.hr_option);
        bind_option(constructor, "msaa_option", &new_options.msaa_option);
        bind_option(constructor, "rr_option", &new_options.rr_option);
        constructor.BindFunc("rr_manual_value",
            [](Rml::Variant& out) {
                out = new_options.rr_manual_value;
            },
            [](const Rml::Variant& in) {
                new_options.rr_manual_value = in.Get<int>();
                graphics_model_handle.DirtyVariable("options_changed");
            });
        constructor.BindFunc("ds_option",
            [](Rml::Variant& out) {
                if (new_options.res_option == ultramodern::renderer::Resolution::Auto) {
                    out = 1;
                } else {
                    out = new_options.ds_option;
                }
            },
            [](const Rml::Variant& in) {
                new_options.ds_option = in.Get<int>();
                graphics_model_handle.DirtyVariable("options_changed");
                graphics_model_handle.DirtyVariable("ds_info");
            });

        constructor.BindFunc("display_refresh_rate",
            [](Rml::Variant& out) {
                out = ultramodern::get_display_refresh_rate();
            });

        constructor.BindFunc("options_changed",
            [](Rml::Variant& out) {
                out = (ultramodern::renderer::get_graphics_config() != new_options);
            });
        constructor.BindFunc("ds_info",
            [](Rml::Variant& out) {
                switch (new_options.res_option) {
                    default:
                    case ultramodern::renderer::Resolution::Auto:
                        out = "Downsampling is not available at auto resolution";
                        return;
                    case ultramodern::renderer::Resolution::Original:
                        if (new_options.ds_option == 2) {
                            out = "Rendered in 480p and scaled to 240p";
                        } else if (new_options.ds_option == 4) {
                            out = "Rendered in 960p and scaled to 240p";
                        }
                        return;
                    case ultramodern::renderer::Resolution::Original2x:
                        if (new_options.ds_option == 2) {
                            out = "Rendered in 960p and scaled to 480p";
                        } else if (new_options.ds_option == 4) {
                            out = "Rendered in 4K and scaled to 480p";
                        }
                        return;
                }
                out = "";
            });
        
        constructor.BindFunc("gfx_help__apply", [](Rml::Variant& out) {
            if (cont_active) {
                out = \
                    (recomp::get_input_binding(recomp::GameInput::APPLY_MENU, 0, recomp::InputDevice::Controller).to_string() != "" ?
                        " " + recomp::get_input_binding(recomp::GameInput::APPLY_MENU, 0, recomp::InputDevice::Controller).to_string() :
                        ""
                    ) + \
                    (recomp::get_input_binding(recomp::GameInput::APPLY_MENU, 1, recomp::InputDevice::Controller).to_string() != "" ?
                        " " + recomp::get_input_binding(recomp::GameInput::APPLY_MENU, 1, recomp::InputDevice::Controller).to_string() :
                        ""
                    );
            } else {
                out = " " PF_KEYBOARD_F;
            }
        });

        constructor.Bind("msaa2x_supported", &msaa2x_supported);
        constructor.Bind("msaa4x_supported", &msaa4x_supported);
        constructor.Bind("msaa8x_supported", &msaa8x_supported);
        constructor.Bind("sample_positions_supported", &sample_positions_supported);

        graphics_model_handle = constructor.GetModelHandle();
    }

    void make_controls_bindings(Rml::Context* context) {
        Rml::DataModelConstructor constructor = context->CreateDataModel("controls_model");
        if (!constructor) {
            throw std::runtime_error("Failed to make RmlUi data model for the controls config menu");
        }

        constructor.BindFunc("input_count", [](Rml::Variant& out) { out = static_cast<uint64_t>(recomp::get_num_inputs()); } );
        constructor.BindFunc("input_device_is_keyboard", [](Rml::Variant& out) { out = cur_device == recomp::InputDevice::Keyboard; } );

        constructor.RegisterTransformFunc("get_input_name", [](const Rml::VariantList& inputs) {
            return Rml::Variant{recomp::get_input_name(static_cast<recomp::GameInput>(inputs.at(0).Get<size_t>()))};
        });

        constructor.RegisterTransformFunc("get_input_enum_name", [](const Rml::VariantList& inputs) {
            return Rml::Variant{recomp::get_input_enum_name(static_cast<recomp::GameInput>(inputs.at(0).Get<size_t>()))};
        });

        constructor.BindEventCallback("set_input_binding",
            [](Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& inputs) {
                scanned_input_index = inputs.at(0).Get<size_t>();
                scanned_binding_index = inputs.at(1).Get<size_t>();
                recomp::start_scanning_input(cur_device);
                model_handle.DirtyVariable("active_binding_input");
                model_handle.DirtyVariable("active_binding_slot");
            });

        constructor.BindEventCallback("reset_input_bindings_to_defaults",
            [](Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& inputs) {
                if (cur_device == recomp::InputDevice::Controller) {
                    zelda64::reset_cont_input_bindings();
                } else {
                    zelda64::reset_kb_input_bindings();
                }
                model_handle.DirtyAllVariables();
                nav_help_model_handle.DirtyVariable("nav_help__accept");
                nav_help_model_handle.DirtyVariable("nav_help__exit");
                graphics_model_handle.DirtyVariable("gfx_help__apply");
            });

        constructor.BindEventCallback("clear_input_bindings",
            [](Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& inputs) {
                recomp::GameInput input = static_cast<recomp::GameInput>(inputs.at(0).Get<size_t>());
                for (size_t binding_index = 0; binding_index < recomp::bindings_per_input; binding_index++) {
                    recomp::set_input_binding(input, binding_index, cur_device, recomp::InputField{});
                }
                model_handle.DirtyVariable("inputs");
                graphics_model_handle.DirtyVariable("gfx_help__apply");
            });

        constructor.BindEventCallback("reset_single_input_binding_to_default",
            [](Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& inputs) {
                recomp::GameInput input = static_cast<recomp::GameInput>(inputs.at(0).Get<size_t>());
                zelda64::reset_single_input_binding(cur_device, input);
                model_handle.DirtyVariable("inputs");
                nav_help_model_handle.DirtyVariable("nav_help__accept");
                nav_help_model_handle.DirtyVariable("nav_help__exit");
            });

        constructor.BindEventCallback("set_input_row_focus",
            [](Rml::DataModelHandle model_handle, Rml::Event& event, const Rml::VariantList& inputs) {
                int input_index = inputs.at(0).Get<size_t>();
                // watch for mouseout being overzealous during event bubbling, only clear if the event's attached element matches the current
                if (input_index == -1 && event.GetType() == "mouseout" && event.GetCurrentElement() != event.GetTargetElement()) {
                    return;
                }
                focused_input_index = input_index;
                model_handle.DirtyVariable("cur_input_row");
            });

        // Rml variable definition for an individual InputField.
        struct InputFieldVariableDefinition : public Rml::VariableDefinition {
            InputFieldVariableDefinition() : Rml::VariableDefinition(Rml::DataVariableType::Scalar) {}

            virtual bool Get(void* ptr, Rml::Variant& variant) override { variant = reinterpret_cast<recomp::InputField*>(ptr)->to_string(); return true; }
            virtual bool Set(void* ptr, const Rml::Variant& variant) override { return false; }
        };
        // Static instance of the InputField variable definition to have a pointer to return to RmlUi.
        static InputFieldVariableDefinition input_field_definition_instance{};

        // Rml variable definition for an array of InputField values (e.g. all the bindings for a single input).
        struct BindingContainerVariableDefinition : public Rml::VariableDefinition {
            BindingContainerVariableDefinition() : Rml::VariableDefinition(Rml::DataVariableType::Array) {}

            virtual bool Get(void* ptr, Rml::Variant& variant) override { return false; }
            virtual bool Set(void* ptr, const Rml::Variant& variant) override { return false; }

            virtual int Size(void* ptr) override { return recomp::bindings_per_input; }
            virtual Rml::DataVariable Child(void* ptr, const Rml::DataAddressEntry& address) override {
                recomp::GameInput input = static_cast<recomp::GameInput>((uintptr_t)ptr);
                return Rml::DataVariable{&input_field_definition_instance, &recomp::get_input_binding(input, address.index, cur_device)};
            }
        };
        // Static instance of the InputField array variable definition to have a fixed pointer to return to RmlUi.
        static BindingContainerVariableDefinition binding_container_var_instance{};

        // Rml variable definition for an array of an array of InputField values (e.g. all the bindings for all inputs).
        struct BindingArrayContainerVariableDefinition : public Rml::VariableDefinition {
            BindingArrayContainerVariableDefinition() : Rml::VariableDefinition(Rml::DataVariableType::Array) {}

            virtual bool Get(void* ptr, Rml::Variant& variant) override { return false; }
            virtual bool Set(void* ptr, const Rml::Variant& variant) override { return false; }

            virtual int Size(void* ptr) override { return recomp::get_num_inputs(); }
            virtual Rml::DataVariable Child(void* ptr, const Rml::DataAddressEntry& address) override {
                // Encode the input index as the pointer to avoid needing to do any allocations.
                return Rml::DataVariable(&binding_container_var_instance, (void*)(uintptr_t)address.index);
            }
        };

        // Static instance of the BindingArrayContainerVariableDefinition variable definition to have a fixed pointer to return to RmlUi.
        static BindingArrayContainerVariableDefinition binding_array_var_instance{};

        struct InputContainerVariableDefinition : public Rml::VariableDefinition {
            InputContainerVariableDefinition() : Rml::VariableDefinition(Rml::DataVariableType::Struct) {}

            virtual bool Get(void* ptr, Rml::Variant& variant) override { return true; }
            virtual bool Set(void* ptr, const Rml::Variant& variant) override { return false; }

            virtual int Size(void* ptr) override { return recomp::get_num_inputs(); }
            virtual Rml::DataVariable Child(void* ptr, const Rml::DataAddressEntry& address) override {
                if (address.name == "array") {
                    return Rml::DataVariable(&binding_array_var_instance, nullptr);
                }
                else {
                    recomp::GameInput input = recomp::get_input_from_enum_name(address.name);
                    if (input != recomp::GameInput::COUNT) {
                        return Rml::DataVariable(&binding_container_var_instance, (void*)(uintptr_t)input);
                    }
                }
                return Rml::DataVariable{};
            }
        };

        // Dummy type to associate with the variable definition.
        struct InputContainer {};
        constructor.RegisterCustomDataVariableDefinition<InputContainer>(Rml::MakeUnique<InputContainerVariableDefinition>());

        // Dummy instance of the dummy type to bind to the variable.
        static InputContainer dummy_container;
        constructor.Bind("inputs", &dummy_container);

        constructor.BindFunc("cur_input_row", [](Rml::Variant& out) {
            if (focused_input_index == -1) {
                out = "NONE";
            }
            else {
                out = recomp::get_input_enum_name(static_cast<recomp::GameInput>(focused_input_index));
            }
        });

        constructor.BindFunc("active_binding_input", [](Rml::Variant& out) {
            if (scanned_input_index == -1) {
                out = "NONE";
            }
            else {
                out = recomp::get_input_enum_name(static_cast<recomp::GameInput>(scanned_input_index));
            }
        });

        constructor.Bind<int>("active_binding_slot", &scanned_binding_index);

        controls_model_handle = constructor.GetModelHandle();
    }

    void make_nav_help_bindings(Rml::Context* context) {
        Rml::DataModelConstructor constructor = context->CreateDataModel("nav_help_model");
        if (!constructor) {
            throw std::runtime_error("Failed to make RmlUi data model for nav help");
        }

        constructor.BindFunc("nav_help__navigate", [](Rml::Variant& out) {
            if (cont_active) {
                out = PF_DPAD;
            } else {
                out = PF_KEYBOARD_ARROWS PF_KEYBOARD_TAB;
            }
        });

        constructor.BindFunc("nav_help__accept", [](Rml::Variant& out) {
            if (cont_active) {
                out = \
                    recomp::get_input_binding(recomp::GameInput::ACCEPT_MENU, 0, recomp::InputDevice::Controller).to_string() + \
                    recomp::get_input_binding(recomp::GameInput::ACCEPT_MENU, 1, recomp::InputDevice::Controller).to_string();
            } else {
                out = PF_KEYBOARD_ENTER;
            }
        });

        constructor.BindFunc("nav_help__exit", [](Rml::Variant& out) {
            if (cont_active) {
                out = \
                    recomp::get_input_binding(recomp::GameInput::TOGGLE_MENU, 0, recomp::InputDevice::Controller).to_string() + \
                    recomp::get_input_binding(recomp::GameInput::TOGGLE_MENU, 1, recomp::InputDevice::Controller).to_string();
            } else {
                out = PF_KEYBOARD_ESCAPE;
            }
        });

        nav_help_model_handle = constructor.GetModelHandle();
    }

    void make_general_bindings(Rml::Context* context) {
        Rml::DataModelConstructor constructor = context->CreateDataModel("general_model");
        if (!constructor) {
            throw std::runtime_error("Failed to make RmlUi data model for the control options menu");
        }

        bind_config_list_events(constructor);
        
        constructor.Bind("rumble_strength", &control_options_context.rumble_strength);
        constructor.Bind("gyro_sensitivity", &control_options_context.gyro_sensitivity);
        constructor.Bind("mouse_sensitivity", &control_options_context.mouse_sensitivity);
        constructor.Bind("joystick_deadzone", &control_options_context.joystick_deadzone);
        bind_option(constructor, "targeting_mode", &control_options_context.targeting_mode);
        bind_option(constructor, "background_input_mode", &control_options_context.background_input_mode);
        bind_option(constructor, "film_grain_mode", &control_options_context.film_grain_mode);
        // Applied immediately rather than only stored, so the renderer hook
        // follows the switch without a restart.
        constructor.BindFunc("widescreen_2d_mode",
            [](Rml::Variant& out) { get_option(control_options_context.widescreen_2d_mode, out); },
            [](const Rml::Variant& in) {
                zelda64::Widescreen2DMode mode = control_options_context.widescreen_2d_mode;
                set_option(mode, in);
                zelda64::set_widescreen_2d_mode(mode);
            });
        bind_option(constructor, "radio_comm_box_mode", &control_options_context.radio_comm_box_mode);
        bind_option(constructor, "invert_y_axis_mode", &control_options_context.invert_y_axis_mode);
        bind_option(constructor, "analog_camera_invert_mode", &control_options_context.analog_camera_invert_mode);

        general_model_handle = constructor.GetModelHandle();
    }
    
    void make_sound_options_bindings(Rml::Context* context) {
        Rml::DataModelConstructor constructor = context->CreateDataModel("sound_options_model");
        if (!constructor) {
            throw std::runtime_error("Failed to make RmlUi data model for the sound options menu");
        }

        bind_config_list_events(constructor);
        
        sound_options_model_handle = constructor.GetModelHandle();

        bind_atomic(constructor, sound_options_model_handle, "main_volume", &sound_options_context.main_volume);
        bind_atomic(constructor, sound_options_model_handle, "bgm_volume", &sound_options_context.bgm_volume);
        bind_atomic(constructor, sound_options_model_handle, "sfx_volume", &sound_options_context.sfx_volume);
		bind_atomic(constructor, sound_options_model_handle, "voice_volume", &sound_options_context.voice_volume);
        bind_atomic(constructor, sound_options_model_handle, "low_health_beeps_enabled", &sound_options_context.low_health_beeps_enabled);
    }

    void make_debug_bindings(Rml::Context* context) {
        Rml::DataModelConstructor constructor = context->CreateDataModel("debug_model");
        if (!constructor) {
            throw std::runtime_error("Failed to make RmlUi data model for the debug menu");
        }

        bind_config_list_events(constructor);

        // Bind the debug mode enabled flag.
        constructor.Bind("debug_enabled", &debug_context.debug_enabled);
        
        // Register the array type for string vectors.
        constructor.RegisterArray<std::vector<std::string>>();
        
        // Bind the warp parameter indices
        constructor.Bind("area_index", &debug_context.area_index);
        constructor.Bind("scene_index", &debug_context.scene_index);
        constructor.Bind("entrance_index", &debug_context.entrance_index);

        // Bind the vectors for warp names
        constructor.Bind("area_names", &debug_context.area_names);
        constructor.Bind("scene_names", &debug_context.scene_names);
        constructor.Bind("entrance_names", &debug_context.entrance_names);

        constructor.Bind("debug_time_day", &debug_context.set_time_day);
        constructor.Bind("debug_time_hour", &debug_context.set_time_hour);
        constructor.Bind("debug_time_minute", &debug_context.set_time_minute);

        debug_context.model_handle = constructor.GetModelHandle();
    }

    void make_cheats_bindings(Rml::Context* context) {
        Rml::DataModelConstructor constructor = context->CreateDataModel("cheats_model");
        if (!constructor) {
            throw std::runtime_error("Failed to make RmlUi data model for the cheats menu");
        }

        bind_config_list_events(constructor);

        constructor.RegisterArray<std::vector<std::string>>();
        init_warp_map_names();
        constructor.Bind("cheat_warp_map_names", &cheats_context.warp_map_names);
        bind_warp_selection(constructor);

        cheats_context.item_names = zelda64::item_names();
        constructor.Bind("cheat_item_names", &cheats_context.item_names);
        constructor.Bind("cheat_item_index", &cheats_context.item_index);
        constructor.BindFunc("cheat_all_enabled",
            [](Rml::Variant& out) { out = cheats_context.all_cheats_enabled ? 1 : 0; },
            [](const Rml::Variant& in) {
                cheats_context.all_cheats_enabled = in.Get<int>() != 0;
                zelda64::set_cheats_enabled(cheats_context.all_cheats_enabled);
            });

        constructor.BindFunc("cheat_stats_available", [](Rml::Variant& out) { out = cheats_context.shown_stats_available; });
        for (const auto& [name, stat] : cheat_stat_names) {
            bind_player_stat(constructor, name, stat);
        }

        cheats_context.model_handle = constructor.GetModelHandle();
    }

    void make_bindings(Rml::Context* context) override {
        // initially set cont state for ui help
        //recomp::config_menu_set_cont_or_kb(recompui::get_cont_active());
        make_nav_help_bindings(context);
        make_general_bindings(context);
        make_controls_bindings(context);
        make_graphics_bindings(context);
        make_sound_options_bindings(context);
        make_debug_bindings(context);
        make_cheats_bindings(context);
        make_enhancements_bindings(context);
        make_speedrun_bindings(context);
        make_randomizer_bindings(context);
        make_audio_bindings(context);
    }
};

std::unique_ptr<recompui::MenuController> recompui::create_config_menu() {
    return std::make_unique<ConfigMenu>();
}

bool zelda64::get_debug_mode_enabled() {
    return debug_context.debug_enabled;
}

void zelda64::set_debug_mode_enabled(bool enabled) {
    debug_context.debug_enabled = enabled;
    if (debug_context.model_handle) {
        debug_context.model_handle.DirtyVariable("debug_enabled");
    }
}

void recompui::update_supported_options() {
    msaa2x_supported = zelda64::renderer::RT64MaxMSAA() >= RT64::UserConfiguration::Antialiasing::MSAA2X;
    msaa4x_supported = zelda64::renderer::RT64MaxMSAA() >= RT64::UserConfiguration::Antialiasing::MSAA4X;
    msaa8x_supported = zelda64::renderer::RT64MaxMSAA() >= RT64::UserConfiguration::Antialiasing::MSAA8X;
    sample_positions_supported = zelda64::renderer::RT64SamplePositionsSupported();
    
    new_options = ultramodern::renderer::get_graphics_config();

    graphics_model_handle.DirtyAllVariables();
}

void recompui::toggle_fullscreen() {
    new_options.wm_option = (new_options.wm_option == ultramodern::renderer::WindowMode::Windowed) ? ultramodern::renderer::WindowMode::Fullscreen : ultramodern::renderer::WindowMode::Windowed;
    apply_graphics_config();
    graphics_model_handle.DirtyVariable("wm_option");
}

void recompui::set_config_tab(ConfigTab tab) {
    get_config_tabset()->SetActiveTab(config_tab_to_index(tab));
}

Rml::ElementTabSet* recompui::get_config_tabset() {
    ContextId config_context = recompui::get_config_context_id();

    ContextId old_context = recompui::try_close_current_context();

    Rml::ElementDocument *doc = config_context.get_document();
    assert(doc != nullptr);

    Rml::Element *tabset_el = doc->GetElementById("config_tabset");
    assert(tabset_el != nullptr);

    Rml::ElementTabSet *tabset = rmlui_dynamic_cast<Rml::ElementTabSet *>(tabset_el);
    assert(tabset != nullptr);

    if (old_context != ContextId::null()) {
        old_context.open();
    }

    return tabset;
}

Rml::Element* recompui::get_mod_tab() {
    ContextId config_context = recompui::get_config_context_id();

    ContextId old_context = recompui::try_close_current_context();

    Rml::ElementDocument* doc = config_context.get_document();
    assert(doc != nullptr);

    Rml::Element* tab_el = doc->GetElementById("tab_mods");
    assert(tab_el != nullptr);

    if (old_context != ContextId::null()) {
        old_context.open();
    }

    return tab_el;
}
