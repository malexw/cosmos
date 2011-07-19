#include "GameObjectManager.hpp"
#include "PlayerInputHandler.hpp"

PlayerInputHandler::PlayerInputHandler(unsigned int id)
 : InputHandler(id), grabbing_(false), rot_(0, -1, -1) {
  collidable_ = GameObjectManager::get().get_object(id)->get_collidable();
  transform_ = GameObjectManager::get().get_object(id)->get_transform();
}

void PlayerInputHandler::handleInput(SDL_Event e) {
  switch (e.type) {
    case SDL_KEYDOWN:
      switch(e.key.keysym.sym) {
        case SDLK_w: velo_.z() = -10; collidable_->set_velocity(velo_); break;
        case SDLK_a: velo_.x() = -10; collidable_->set_velocity(velo_); break;
        case SDLK_s: velo_.z() = 10; collidable_->set_velocity(velo_); break;
        case SDLK_d: velo_.x() = 10; collidable_->set_velocity(velo_); break;
      };
      break;
    case SDL_KEYUP:
      switch(e.key.keysym.sym) {
        case SDLK_w: velo_.z() = 0; collidable_->set_velocity(velo_); break;
        case SDLK_a: velo_.x() = 0; collidable_->set_velocity(velo_); break;
        case SDLK_s: velo_.z() = 0; collidable_->set_velocity(velo_); break;
        case SDLK_d: velo_.x() = 0; collidable_->set_velocity(velo_); break;
      };
      break;
    case SDL_MOUSEMOTION:
      if (grabbing_) {
        //std::cout << "Mouse " << e.motion.xrel << " " << e.motion.yrel << std::endl;
        transform_->rotate(Vector3f::NEGATIVE_Y, e.motion.xrel);
        transform_->rotate_relative(Vector3f::NEGATIVE_X, e.motion.yrel);
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
      SDL_Quit();
      exit(0);
      break;
  }
}
