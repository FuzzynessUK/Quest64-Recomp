#include <array>

#include "SDL.h"

#include "librecomp/helpers.hpp"
#include "recomp_input.h"
#include "ultramodern/ultramodern.hpp"
#include "zelda_game.h"
#include "enhancements.h"
#include "minimap.h"
#include "traps.h"

// Arrays that hold the mappings for every input for keyboard and controller respectively.
using input_mapping = std::array<recomp::InputField, recomp::bindings_per_input>;
using input_mapping_array = std::array<input_mapping, static_cast<size_t>(recomp::GameInput::COUNT)>;
static input_mapping_array keyboard_input_mappings{};
static input_mapping_array controller_input_mappings{};

// Make the button value array, which maps a button index to its bit field.
#define DEFINE_INPUT(name, value, readable) uint16_t(value##u),
static const std::array n64_button_values = {
    DEFINE_N64_BUTTON_INPUTS()
};
#undef DEFINE_INPUT

// Make the input name array.
#define DEFINE_INPUT(name, value, readable) readable,
static const std::vector<std::string> input_names = {
    DEFINE_ALL_INPUTS()
};
#undef DEFINE_INPUT

// Make the input enum name array.
#define DEFINE_INPUT(name, value, readable) #name,
static const std::vector<std::string> input_enum_names = {
    DEFINE_ALL_INPUTS()
};
#undef DEFINE_INPUT

// The hotkeys below (Reset Game, Exit Spell, the minimap zooms) are read even
// with the config menu open, which a controller moves around with the D-pad;
// a hotkey bound to the D-pad would fire while navigating it (Reset Game
// would restart the game). So while game input is off, a hotkey's controller
// D-pad bindings do not count. In play they work like any other button,
// which is what the D-pad is for now that Quest 64's own D-pad rows are gone.
static bool hotkey_held(recomp::GameInput input) {
    const size_t index = static_cast<size_t>(input);
    if (recomp::get_input_digital(keyboard_input_mappings[index])) {
        return true;
    }
    for (const recomp::InputField& field : controller_input_mappings[index]) {
        // InputType::ControllerDigital is 3 (input.cpp keeps that enum private).
        const bool dpad = field.input_type == 3 &&
            field.input_id >= SDL_CONTROLLER_BUTTON_DPAD_UP && field.input_id <= SDL_CONTROLLER_BUTTON_DPAD_RIGHT;
        if (dpad && recomp::game_input_disabled()) {
            continue;
        }
        if (recomp::get_input_digital(field)) {
            return true;
        }
    }
    return false;
}

size_t recomp::get_num_inputs() {
    return (size_t)GameInput::COUNT;
}

const std::string& recomp::get_input_name(GameInput input) {
    return input_names.at(static_cast<size_t>(input));
}

const std::string& recomp::get_input_enum_name(GameInput input) {
    return input_enum_names.at(static_cast<size_t>(input));
}

recomp::GameInput recomp::get_input_from_enum_name(const std::string_view enum_name) {
    auto find_it = std::find(input_enum_names.begin(), input_enum_names.end(), enum_name);
    if (find_it == input_enum_names.end()) {
        return recomp::GameInput::COUNT;
    }

    return static_cast<recomp::GameInput>(find_it - input_enum_names.begin());
}

// Due to an RmlUi limitation this can't be const. Ideally it would return a const reference or even just a straight up copy.
recomp::InputField& recomp::get_input_binding(GameInput input, size_t binding_index, recomp::InputDevice device) {
    input_mapping_array& device_mappings = (device == recomp::InputDevice::Controller) ?  controller_input_mappings : keyboard_input_mappings;
    input_mapping& cur_input_mapping = device_mappings.at(static_cast<size_t>(input));

    if (binding_index < cur_input_mapping.size()) {
        return cur_input_mapping[binding_index];
    }
    else {
        static recomp::InputField dummy_field = {};
        return dummy_field;
    }
}

void recomp::set_input_binding(recomp::GameInput input, size_t binding_index, recomp::InputDevice device, recomp::InputField value) {
    input_mapping_array& device_mappings = (device == recomp::InputDevice::Controller) ?  controller_input_mappings : keyboard_input_mappings;
    input_mapping& cur_input_mapping = device_mappings.at(static_cast<size_t>(input));

    if (binding_index < cur_input_mapping.size()) {
        cur_input_mapping[binding_index] = value;
    }
}

bool recomp::get_n64_input(int controller_num, uint16_t* buttons_out, float* x_out, float* y_out) {
    uint16_t cur_buttons = 0;
    float cur_x = 0.0f;
    float cur_y = 0.0f;
    
    if (controller_num != 0) {
        return false;
    }

    // Reset is checked outside the game-input guard so the hotkey still works
    // with the menu open, and on the press edge so holding it only fires once.
    {
        static bool reset_was_held = false;
        bool reset_held = hotkey_held(GameInput::RESET_GAME);
        if (reset_held && !reset_was_held) {
            zelda64::restart_application();
        }
        reset_was_held = reset_held;
    }

    // Exit works from anywhere, with or without the spell, so it is checked
    // here rather than gated on the menu being open.
    {
        static bool exit_was_held = false;
        bool exit_held = hotkey_held(GameInput::EXIT_SPELL);
        if (exit_held && !exit_was_held) {
            zelda64::enhancements::cast_exit();
        }
        exit_was_held = exit_held;
    }

    // Minimap zoom, one step a press. The minimap takes them the next time it
    // draws, and only while it is on.
    {
        static bool zoom_was_held[2] = { false, false };
        const GameInput zoom_inputs[2] = { GameInput::MINIMAP_ZOOM_IN, GameInput::MINIMAP_ZOOM_OUT };
        for (int k = 0; k < 2; k++) {
            bool held = hotkey_held(zoom_inputs[k]);
            if (held && !zoom_was_held[k]) {
                zelda64::minimap::zoom(k == 0 ? 1 : -1);
            }
            zoom_was_held[k] = held;
        }
    }

    if (!recomp::game_input_disabled()) {
        for (size_t i = 0; i < n64_button_values.size(); i++) {
            size_t input_index = (size_t)GameInput::N64_BUTTON_START + i;
            cur_buttons |= recomp::get_input_digital(keyboard_input_mappings[input_index]) ? n64_button_values[i] : 0;
            cur_buttons |= recomp::get_input_digital(controller_input_mappings[input_index]) ? n64_button_values[i] : 0;
        }

        float joystick_deadzone = recomp::get_joystick_deadzone() / 100.0f;

        float joystick_x = recomp::get_input_analog(controller_input_mappings[(size_t)GameInput::X_AXIS_POS])
                        - recomp::get_input_analog(controller_input_mappings[(size_t)GameInput::X_AXIS_NEG]);

        float joystick_y = recomp::get_input_analog(controller_input_mappings[(size_t)GameInput::Y_AXIS_POS])
                        - recomp::get_input_analog(controller_input_mappings[(size_t)GameInput::Y_AXIS_NEG]);

        recomp::apply_joystick_deadzone(joystick_x, joystick_y, &joystick_x, &joystick_y);

        cur_x = recomp::get_input_analog(keyboard_input_mappings[(size_t)GameInput::X_AXIS_POS])
                - recomp::get_input_analog(keyboard_input_mappings[(size_t)GameInput::X_AXIS_NEG]) + joystick_x;

        cur_y = recomp::get_input_analog(keyboard_input_mappings[(size_t)GameInput::Y_AXIS_POS])
                - recomp::get_input_analog(keyboard_input_mappings[(size_t)GameInput::Y_AXIS_NEG]) + joystick_y;
    }

    // Ice Trap: Brian is frozen, so nothing he could do with A, B, Z or the
    // stick reaches the game (Start still opens the menu).
    if (zelda64::traps::input_locked()) {
        constexpr uint16_t a_b_z = 0x8000 | 0x4000 | 0x2000;
        cur_buttons &= static_cast<uint16_t>(~a_b_z);
        cur_x = 0.0f;
        cur_y = 0.0f;
    }

    *buttons_out = cur_buttons;
    *x_out = std::clamp(cur_x * 0.65f, -1.0f, 1.0f);
    *y_out = std::clamp(cur_y * 0.65f, -1.0f, 1.0f);

    return true;
}
