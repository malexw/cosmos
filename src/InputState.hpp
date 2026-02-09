#ifndef COSMOS_INPUTSTATE_HPP_
#define COSMOS_INPUTSTATE_HPP_

#include <unordered_map>
#include <unordered_set>

#include <SDL3/SDL.h>

class InputState {
public:
    void begin_frame();
    void update(const SDL_Event& e);

    // Keyboard
    bool key_held(SDL_Keycode key) const;
    bool key_just_pressed(SDL_Keycode key) const;
    bool key_just_released(SDL_Keycode key) const;

    // Mouse buttons
    bool mouse_held(Uint8 button) const;
    bool mouse_just_pressed(Uint8 button) const;
    bool mouse_just_released(Uint8 button) const;
    float mouse_delta_x() const;
    float mouse_delta_y() const;

    // Gamepad buttons
    bool gamepad_held(SDL_GamepadButton button) const;
    bool gamepad_just_pressed(SDL_GamepadButton button) const;
    bool gamepad_just_released(SDL_GamepadButton button) const;

    // Gamepad axes — raw normalized value [-1, 1]
    float gamepad_axis(SDL_GamepadAxis axis) const;

    bool quit_requested() const;

private:
    static constexpr float DEAD_ZONE = 0.15f;

    std::unordered_set<SDL_Keycode> keys_held_;
    std::unordered_set<SDL_Keycode> keys_pressed_;
    std::unordered_set<SDL_Keycode> keys_released_;

    std::unordered_set<Uint8> mouse_held_;
    std::unordered_set<Uint8> mouse_pressed_;
    std::unordered_set<Uint8> mouse_released_;
    float mouse_dx_ = 0.0f;
    float mouse_dy_ = 0.0f;

    std::unordered_set<int> gamepad_held_;
    std::unordered_set<int> gamepad_pressed_;
    std::unordered_set<int> gamepad_released_;
    std::unordered_map<int, float> gamepad_axes_;

    bool quit_ = false;
};

#endif
