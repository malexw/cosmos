#include "InputManager.hpp"

#include <imgui_impl_sdl3.h>
#include <SDL3/SDL.h>

void InputManager::update() {
    input_state_.begin_frame();
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        input_state_.update(e);
        ImGui_ImplSDL3_ProcessEvent(&e);
    }
    action_state_ = action_map_.resolve(input_state_, action_state_);
}
