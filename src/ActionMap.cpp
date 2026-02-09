#include "ActionMap.hpp"

#include <algorithm>
#include <cmath>

ActionMap::ActionMap() {
    // Discrete action bindings
    action_bindings_.push_back({Action::ToggleDebugMenu, {PhysicalInput::from_key(SDLK_F12)}});
    action_bindings_.push_back({Action::ToggleDebugMenu, {
        PhysicalInput::from_gamepad(SDL_GAMEPAD_BUTTON_LEFT_SHOULDER),
        PhysicalInput::from_gamepad(SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER)
    }});
    action_bindings_.push_back({Action::Quit, {PhysicalInput::from_key(SDLK_ESCAPE)}});
    action_bindings_.push_back({Action::Grab, {PhysicalInput::from_mouse_button(SDL_BUTTON_RIGHT)}});

    // Key → Axis bindings
    key_axis_bindings_.push_back({Axis::MoveY, SDLK_W, -1.0f});
    key_axis_bindings_.push_back({Axis::MoveY, SDLK_S,  1.0f});
    key_axis_bindings_.push_back({Axis::MoveX, SDLK_A, -1.0f});
    key_axis_bindings_.push_back({Axis::MoveX, SDLK_D,  1.0f});

    // Stick → Axis bindings
    stick_axis_bindings_.push_back({Axis::MoveX, SDL_GAMEPAD_AXIS_LEFTX,  1.0f});
    stick_axis_bindings_.push_back({Axis::MoveY, SDL_GAMEPAD_AXIS_LEFTY,  1.0f});
    stick_axis_bindings_.push_back({Axis::LookX, SDL_GAMEPAD_AXIS_RIGHTX, 1.0f});
    stick_axis_bindings_.push_back({Axis::LookY, SDL_GAMEPAD_AXIS_RIGHTY, 1.0f});

    // Mouse → Axis bindings
    mouse_axis_bindings_.push_back({Axis::MouseLookX, false, 1.0f});
    mouse_axis_bindings_.push_back({Axis::MouseLookY, true,  1.0f});
}

ActionState ActionMap::resolve(const InputState& input, const ActionState& prev) const {
    ActionState state;

    // Resolve discrete actions
    for (const auto& binding : action_bindings_) {
        bool all_held = true;
        bool any_just_pressed = false;

        for (const auto& pi : binding.inputs) {
            switch (pi.type) {
                case PhysicalInput::Type::Key:
                    if (!input.key_held(pi.key)) all_held = false;
                    if (input.key_just_pressed(pi.key)) any_just_pressed = true;
                    break;
                case PhysicalInput::Type::MouseButton:
                    if (!input.mouse_held(pi.mouse_button)) all_held = false;
                    if (input.mouse_just_pressed(pi.mouse_button)) any_just_pressed = true;
                    break;
                case PhysicalInput::Type::GamepadButton:
                    if (!input.gamepad_held(pi.gamepad_button)) all_held = false;
                    if (input.gamepad_just_pressed(pi.gamepad_button)) any_just_pressed = true;
                    break;
            }
        }

        if (all_held) {
            state.active_.insert(binding.action);
            // Trigger on the frame where the last input completes the chord
            if (any_just_pressed && !prev.is_active(binding.action)) {
                state.triggered_.insert(binding.action);
            }
        }
    }

    // Resolve axes
    std::unordered_map<Axis, float, AxisHash> axis_accum;

    for (const auto& kb : key_axis_bindings_) {
        if (input.key_held(kb.key)) {
            axis_accum[kb.axis] += kb.value;
        }
    }

    for (const auto& sb : stick_axis_bindings_) {
        float raw = input.gamepad_axis(sb.sdl_axis);
        axis_accum[sb.axis] += raw * sb.scale;
    }

    for (const auto& mb : mouse_axis_bindings_) {
        float raw = mb.use_delta_y ? input.mouse_delta_y() : input.mouse_delta_x();
        axis_accum[mb.axis] += raw * mb.scale;
    }

    for (auto& [axis, value] : axis_accum) {
        state.axes_[axis] = std::clamp(value, -1.0f, 1.0f);
    }

    return state;
}
