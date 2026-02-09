#include "InputState.hpp"

#include <algorithm>
#include <cmath>

void InputState::begin_frame() {
    keys_pressed_.clear();
    keys_released_.clear();
    mouse_pressed_.clear();
    mouse_released_.clear();
    gamepad_pressed_.clear();
    gamepad_released_.clear();
    mouse_dx_ = 0.0f;
    mouse_dy_ = 0.0f;
    quit_ = false;
}

void InputState::update(const SDL_Event& e) {
    switch (e.type) {
        case SDL_EVENT_KEY_DOWN:
            if (!e.key.repeat) {
                keys_held_.insert(e.key.key);
                keys_pressed_.insert(e.key.key);
            }
            break;
        case SDL_EVENT_KEY_UP:
            keys_held_.erase(e.key.key);
            keys_released_.insert(e.key.key);
            break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            mouse_held_.insert(e.button.button);
            mouse_pressed_.insert(e.button.button);
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            mouse_held_.erase(e.button.button);
            mouse_released_.insert(e.button.button);
            break;
        case SDL_EVENT_MOUSE_MOTION:
            mouse_dx_ += e.motion.xrel;
            mouse_dy_ += e.motion.yrel;
            break;

        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            gamepad_held_.insert(e.gbutton.button);
            gamepad_pressed_.insert(e.gbutton.button);
            break;
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            gamepad_held_.erase(e.gbutton.button);
            gamepad_released_.insert(e.gbutton.button);
            break;
        case SDL_EVENT_GAMEPAD_AXIS_MOTION: {
            float normalized = static_cast<float>(e.gaxis.value) / 32767.0f;
            normalized = std::clamp(normalized, -1.0f, 1.0f);
            if (std::fabs(normalized) < DEAD_ZONE) {
                normalized = 0.0f;
            }
            gamepad_axes_[e.gaxis.axis] = normalized;
            break;
        }

        case SDL_EVENT_QUIT:
        case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
            quit_ = true;
            break;

        default:
            break;
    }
}

bool InputState::key_held(SDL_Keycode key) const {
    return keys_held_.count(key) > 0;
}

bool InputState::key_just_pressed(SDL_Keycode key) const {
    return keys_pressed_.count(key) > 0;
}

bool InputState::key_just_released(SDL_Keycode key) const {
    return keys_released_.count(key) > 0;
}

bool InputState::mouse_held(Uint8 button) const {
    return mouse_held_.count(button) > 0;
}

bool InputState::mouse_just_pressed(Uint8 button) const {
    return mouse_pressed_.count(button) > 0;
}

bool InputState::mouse_just_released(Uint8 button) const {
    return mouse_released_.count(button) > 0;
}

float InputState::mouse_delta_x() const { return mouse_dx_; }
float InputState::mouse_delta_y() const { return mouse_dy_; }

bool InputState::gamepad_held(SDL_GamepadButton button) const {
    return gamepad_held_.count(static_cast<int>(button)) > 0;
}

bool InputState::gamepad_just_pressed(SDL_GamepadButton button) const {
    return gamepad_pressed_.count(static_cast<int>(button)) > 0;
}

bool InputState::gamepad_just_released(SDL_GamepadButton button) const {
    return gamepad_released_.count(static_cast<int>(button)) > 0;
}

float InputState::gamepad_axis(SDL_GamepadAxis axis) const {
    auto it = gamepad_axes_.find(static_cast<int>(axis));
    return (it != gamepad_axes_.end()) ? it->second : 0.0f;
}

bool InputState::quit_requested() const { return quit_; }
