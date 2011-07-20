#include "GameObjectManager.hpp"
#include "PlayerInputHandler.hpp"
#include "ResourceManager/ShaderManager.hpp"

PlayerInputHandler::PlayerInputHandler(unsigned int id)
 : InputHandler(id), grabbing_(false), rot_(0, -1, -1), config_(CosmosConfig::get()) {
  collidable_ = GameObjectManager::get().get_object(id)->get_collidable();
  transform_ = GameObjectManager::get().get_object(id)->get_transform();
  hdr_program_ = ShaderManager::get().get_shader_program("hdr");
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
        case SDLK_c: config_.set_collidables(!config_.is_collidables()); break;
        case SDLK_b: config_.set_bump_mapping(!config_.is_bump_mapping()); break;
        case SDLK_t: config_.set_textures(!config_.is_textures()); break;
        case SDLK_h: config_.set_hdr(!config_.is_hdr()); break;
        case SDLK_y: config_.set_decals(!config_.is_decals()); break;
        case SDLK_p: config_.set_particles(!config_.is_particles()); break;
        // HDR exposure
        case SDLK_1: hdr_program_->setf(std::string("exposure"), 1.0f); break;
        case SDLK_2: hdr_program_->setf(std::string("exposure"), 2.0f); break;
        case SDLK_3: hdr_program_->setf(std::string("exposure"), 3.0f); break;
        case SDLK_4: hdr_program_->setf(std::string("exposure"), 4.0f); break;
        case SDLK_5: hdr_program_->setf(std::string("exposure"), 5.0f); break;
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
