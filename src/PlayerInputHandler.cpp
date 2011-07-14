#include "PlayerInputHandler.hpp"

void PlayerInputHandler::handleInput(SDL_Event e) {
  switch (e.type) {
    case SDL_KEYDOWN:
      switch(e.key.keysym.sym) {
        case SDLK_w: velo_.z() = -10; listener_->set_velocity(velo_); break;
        case SDLK_a: velo_.x() = -10; listener_->set_velocity(velo_); break;
        case SDLK_s: velo_.z() = 10; listener_->set_velocity(velo_); break;
        case SDLK_d: velo_.x() = 10; listener_->set_velocity(velo_); break;
      };
      break;
    case SDL_KEYUP:
      switch(e.key.keysym.sym) {
        case SDLK_w: velo_.z() = 0; listener_->set_velocity(velo_); break;
        case SDLK_a: velo_.x() = 0; listener_->set_velocity(velo_); break;
        case SDLK_s: velo_.z() = 0; listener_->set_velocity(velo_); break;
        case SDLK_d: velo_.x() = 0; listener_->set_velocity(velo_); break;
      };
      break;
    case SDL_MOUSEMOTION:
      if (grabbing_) { //std::cout << "Mouse " << e.motion.xrel << " " << e.motion.yrel << std::endl;
        //rot_.x() += e.motion.xrel/10.0f;
        //rot_.y() -= e.motion.yrel/10.0f;
        listener_->rotate(Vector3f(0, -1.0, 0), e.motion.xrel);
        listener_->rotate(Vector3f(-1.0, 0, 0), e.motion.yrel);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      switch(e.button.button) {
        case SDL_BUTTON_RIGHT: SDL_WM_GrabInput(SDL_GRAB_ON); SDL_ShowCursor(SDL_DISABLE); grabbing_ = true; break;
      }
      break;
    case SDL_MOUSEBUTTONUP:
      switch(e.button.button) {
        case SDL_BUTTON_RIGHT: SDL_WM_GrabInput(SDL_GRAB_OFF); SDL_ShowCursor(SDL_ENABLE); grabbing_ = false; break;
      }
      break;
    case SDL_QUIT:
        exit(0);
  }
}
