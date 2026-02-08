#include "PlayerInputHandler.hpp"

PlayerInputHandler::PlayerInputHandler(Transform::ShPtr transform, CollidableObject::ShPtr collidable)
 : InputHandler(0), transform_(transform), collidable_(collidable),
   rot_(0, -1, -1), grabbing_(false), config_(CosmosConfig::get()) {
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
        // Config stuff
        case SDLK_v: config_.set_collidables(!config_.is_collidables()); break;
        case SDLK_c: config_.set_collisions(!config_.is_collisions()); break;
        case SDLK_b: config_.set_bump_mapping(!config_.is_bump_mapping()); break;
        case SDLK_t: config_.set_textures(!config_.is_textures()); break;
        case SDLK_k: config_.set_skybox(!config_.is_skybox()); break;
        case SDLK_y: config_.set_decals(!config_.is_decals()); break;
        case SDLK_p: config_.set_particles(!config_.is_particles()); break;
        case SDLK_n: config_.set_shadows(!config_.is_shadows()); break;
        case SDLK_m: config_.set_sounds(!config_.is_sounds()); break;
        case SDLK_f: config_.set_shadow_debug(!config_.is_shadow_debug()); break;
        case SDLK_g: config_.set_pcf_mode((config_.pcf_mode() + 1) % 3); break;
        case SDLK_x: config_.set_show_axes(!config_.is_show_axes()); break;
        case SDLK_l: {
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
    case SDL_MOUSEMOTION:
      if (grabbing_) {
        //std::cout << "Mouse " << e.motion.xrel << " " << e.motion.yrel << std::endl;
        transform_->rotate(Vector3f::NEGATIVE_Y, e.motion.xrel);
        transform_->rotate_relative(Vector3f::NEGATIVE_X, e.motion.yrel);
      }
      break;
    case SDL_MOUSEBUTTONDOWN:
      switch(e.button.button) {
        case SDL_BUTTON_RIGHT: SDL_SetRelativeMouseMode(SDL_TRUE); grabbing_ = true; break;
      }
      break;
    case SDL_MOUSEBUTTONUP:
      switch(e.button.button) {
        case SDL_BUTTON_RIGHT: SDL_SetRelativeMouseMode(SDL_FALSE); grabbing_ = false; break;
      }
      break;
    case SDL_QUIT:
      config_.set_quit(true);
      break;
  }
}
