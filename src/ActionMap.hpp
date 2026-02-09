#ifndef COSMOS_ACTIONMAP_HPP_
#define COSMOS_ACTIONMAP_HPP_

#include <vector>

#include <SDL3/SDL.h>

#include "Action.hpp"
#include "ActionState.hpp"
#include "InputState.hpp"

class ActionMap {
public:
    ActionMap();
    ActionState resolve(const InputState& input, const ActionState& prev) const;

private:
    struct PhysicalInput {
        enum class Type { Key, MouseButton, GamepadButton };
        Type type;
        union {
            SDL_Keycode key;
            Uint8 mouse_button;
            SDL_GamepadButton gamepad_button;
        };

        static PhysicalInput from_key(SDL_Keycode k) {
            PhysicalInput p; p.type = Type::Key; p.key = k; return p;
        }
        static PhysicalInput from_mouse_button(Uint8 b) {
            PhysicalInput p; p.type = Type::MouseButton; p.mouse_button = b; return p;
        }
        static PhysicalInput from_gamepad(SDL_GamepadButton b) {
            PhysicalInput p; p.type = Type::GamepadButton; p.gamepad_button = b; return p;
        }
    };

    struct ActionBinding {
        Action action;
        std::vector<PhysicalInput> inputs;
    };

    struct KeyAxisBinding {
        Axis axis;
        SDL_Keycode key;
        float value;
    };

    struct StickAxisBinding {
        Axis axis;
        SDL_GamepadAxis sdl_axis;
        float scale;
    };

    struct MouseAxisBinding {
        Axis axis;
        bool use_delta_y;  // false = delta X, true = delta Y
        float scale;
    };

    std::vector<ActionBinding> action_bindings_;
    std::vector<KeyAxisBinding> key_axis_bindings_;
    std::vector<StickAxisBinding> stick_axis_bindings_;
    std::vector<MouseAxisBinding> mouse_axis_bindings_;
};

#endif
