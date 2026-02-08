#include "PlayerInputHandler.hpp"

PlayerInputHandler::PlayerInputHandler(SDL_Window* window, Transform::ShPtr transform, CollidableObject::ShPtr collidable)
 : InputHandler(0), window_(window), rot_(0, -1, -1), collidable_(collidable),
   transform_(transform), grabbing_(false), config_(CosmosConfig::get()) {
}

void PlayerInputHandler::handleInput(SDL_Event e) {
  switch (e.type) {
    case SDL_EVENT_KEY_DOWN:
      switch(e.key.key) {
        case SDLK_W: velo_.z() = -10; collidable_->set_velocity(velo_); break;
        case SDLK_A: velo_.x() = -10; collidable_->set_velocity(velo_); break;
        case SDLK_S: velo_.z() = 10; collidable_->set_velocity(velo_); break;
        case SDLK_D: velo_.x() = 10; collidable_->set_velocity(velo_); break;
      };
      break;
    case SDL_EVENT_KEY_UP:
      switch(e.key.key) {
        case SDLK_W: velo_.z() = 0; collidable_->set_velocity(velo_); break;
        case SDLK_A: velo_.x() = 0; collidable_->set_velocity(velo_); break;
        case SDLK_S: velo_.z() = 0; collidable_->set_velocity(velo_); break;
        case SDLK_D: velo_.x() = 0; collidable_->set_velocity(velo_); break;
        // Config stuff
        case SDLK_V: config_.set_collidables(!config_.is_collidables()); break;
        case SDLK_C: config_.set_collisions(!config_.is_collisions()); break;
        case SDLK_B: config_.set_bump_mapping(!config_.is_bump_mapping()); break;
        case SDLK_T: config_.set_textures(!config_.is_textures()); break;
        case SDLK_K: config_.set_skybox(!config_.is_skybox()); break;
        case SDLK_Y: config_.set_decals(!config_.is_decals()); break;
        case SDLK_P: config_.set_particles(!config_.is_particles()); break;
        case SDLK_N: config_.set_shadows(!config_.is_shadows()); break;
        case SDLK_M: config_.set_sounds(!config_.is_sounds()); break;
        case SDLK_F: config_.set_shadow_debug(!config_.is_shadow_debug()); break;
        case SDLK_G: config_.set_pcf_mode((config_.pcf_mode() + 1) % 3); break;
        case SDLK_X: config_.set_show_axes(!config_.is_show_axes()); break;
        case SDLK_L: {
            Vector3f pos = transform_->get_position();
            Vector3f dir = (transform_->get_rotation() * Vector3f::NEGATIVE_Z).normalize();
            std::cout << "Camera position:   " << pos << std::endl;
            std::cout << "Camera direction:  " << dir << std::endl;
            std::cout << "Inverse direction: " << -dir << std::endl;
            break;
        }
        case SDLK_ESCAPE: config_.set_quit(true); break;
        // HDR exposure
        case SDLK_1: config_.set_exposure(1.0f); break;
        case SDLK_2: config_.set_exposure(2.0f); break;
        case SDLK_3: config_.set_exposure(6.0f); break;
        case SDLK_4: config_.set_exposure(8.0f); break;
        case SDLK_5: config_.set_exposure(10.0f); break;
      };
      break;
    case SDL_EVENT_MOUSE_MOTION:
      if (grabbing_) {
        //std::cout << "Mouse " << e.motion.xrel << " " << e.motion.yrel << std::endl;
        transform_->rotate(Vector3f::NEGATIVE_Y, e.motion.xrel);
        transform_->rotate_relative(Vector3f::NEGATIVE_X, e.motion.yrel);
      }
      break;
    case SDL_EVENT_MOUSE_BUTTON_DOWN:
      switch(e.button.button) {
        case SDL_BUTTON_RIGHT: SDL_SetWindowRelativeMouseMode(window_, true); grabbing_ = true; break;
      }
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      switch(e.button.button) {
        case SDL_BUTTON_RIGHT: SDL_SetWindowRelativeMouseMode(window_, false); grabbing_ = false; break;
      }
      break;
    case SDL_EVENT_QUIT:
      config_.set_quit(true);
      break;
  }
}
