#include "PlayerInputHandler.hpp"

void PlayerInputHandler::handleInput(SDL_Event e) {
  switch (e.type) {
    case SDL_KEYDOWN:
      switch(e.key.keysym.sym) {
        case SDLK_w: pos_.z() -= 1; listener_->set_translate(pos_); break;
        case SDLK_a: pos_.x() -= 1; listener_->set_translate(pos_); break;
        case SDLK_s: pos_.z() += 1; listener_->set_translate(pos_); break;
        case SDLK_d: pos_.x() += 1; listener_->set_translate(pos_); break;
      };
      break;
    case SDL_QUIT:
        exit(0);
  }
}
