#include "GameObjectManager.hpp"
#include "PlayerInputHandler.hpp"
#include "ResourceManager/ShaderManager.hpp"

PlayerInputHandler::PlayerInputHandler(GameObjectManager::ShPtr gob_man, unsigned int id)
 : InputHandler(id), gob_man_(gob_man), camera_id_(id), grabbing_(false), rot_(0, -1, -1), config_(CosmosConfig::get()) {
}

void PlayerInputHandler::handleInput(SDL_Event e) {
  Message::ShPtr mcv(new Message(Message::COLLIDABLE_VELOCITY));
  Message::ShPtr mtu(new Message(Message::TRANSFORM_UPDATE));
  switch (e.type) {
    case SDL_KEYDOWN:
      switch(e.key.keysym.sym) {
        case SDLK_w: velo_.z() = -10; break;
        case SDLK_a: velo_.x() = -10; break;
        case SDLK_s: velo_.z() = 10; break;
        case SDLK_d: velo_.x() = 10; break;
      };
      mcv->add_arg("velocity_x", velo_.x()).add_arg("velocity_y", velo_.y()).add_arg("velocity_z", velo_.z());
      gob_man_->message_collidable(camera_id_, mcv);
      break;
    case SDL_KEYUP:
      switch(e.key.keysym.sym) {
        case SDLK_w: velo_.z() = 0; break;
        case SDLK_a: velo_.x() = 0; break;
        case SDLK_s: velo_.z() = 0; break;
        case SDLK_d: velo_.x() = 0; break;
        // Config stuff
        case SDLK_v: config_.set_collidables(!config_.is_collidables()); break;
        case SDLK_c: config_.set_collisions(!config_.is_collisions()); break;
        case SDLK_b: config_.set_bump_mapping(!config_.is_bump_mapping()); break;
        case SDLK_t: config_.set_textures(!config_.is_textures()); break;
        case SDLK_k: config_.set_skybox(!config_.is_skybox()); break;
        case SDLK_h: config_.set_hdr(!config_.is_hdr()); break;
        case SDLK_y: config_.set_decals(!config_.is_decals()); break;
        case SDLK_p: config_.set_particles(!config_.is_particles()); break;
        case SDLK_n: config_.set_shadows(!config_.is_shadows()); break;
        case SDLK_m: config_.set_sounds(!config_.is_sounds()); break;
        case SDLK_ESCAPE: config_.set_quit(true); break;
        // HDR exposure
        //case SDLK_1: hdr_program_->setf(std::string("exposure"), 1.0f); break;
        //case SDLK_2: hdr_program_->setf(std::string("exposure"), 2.0f); break;
        //case SDLK_3: hdr_program_->setf(std::string("exposure"), 6.0f); break;
        //case SDLK_4: hdr_program_->setf(std::string("exposure"), 8.0f); break;
        //case SDLK_5: hdr_program_->setf(std::string("exposure"), 10.0f); break;
      };
      mcv->add_arg("velocity_x", velo_.x()).add_arg("velocity_y", velo_.y()).add_arg("velocity_z", velo_.z());
      gob_man_->message_collidable(camera_id_, mcv);
      break;
    case SDL_MOUSEMOTION:
      if (grabbing_) {
        //std::cout << "Mouse " << e.motion.xrel << " " << e.motion.yrel << std::endl;
        //transform_->rotate(Vector3f::NEGATIVE_Y, e.motion.xrel);
        //transform_->rotate_relative(Vector3f::NEGATIVE_X, e.motion.yrel);
        //Quaternion q1 = Quaternion(Vector3f::NEGATIVE_Y, e.motion.xrel);
        //Quaternion q2 = Quaternion((q1 * Vector3f::NEGATIVE_X), e.motion.yrel);
        mtu->add_arg("yaw", e.motion.xrel).add_arg("pitch", e.motion.yrel);
        gob_man_->message_transform(camera_id_, mtu);
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
      config_.set_quit(true);
      break;
  }
}
