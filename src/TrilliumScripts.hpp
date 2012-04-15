#ifndef TRILLIUM_TRILLIUMSCRIPTS_H_
#define TRILLIUM_TRILLIUMSCRIPTS_H_

#include <iostream>

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include "SDL/SDL.h"

#include "GameObjectManager.hpp"
#include "Message.hpp"
#include "util.hpp"

// If I had the Lua engine running, all these "scripts" would be in Lua
// The code looks wonky because I'm trying to build a logical separation between the contents of this file and the rest of the engine

class TrilliumScripts: public boost::enable_shared_from_this<TrilliumScripts> {
 public:
  typedef boost::shared_ptr<TrilliumScripts> ShPtr;

  TrilliumScripts(GameObjectManager::ShPtr gob_manager, Camera::ShPtr camera)
   : gob_manager_(gob_manager), camera_(camera) {
  }

  void init() {
    // pass in the camera here so I don't have to write code to create them here... 4 hours to go.
    // Create cameras?

    // Create meteor field

    // Create the spaceships
    player_ss_id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE | GameObjectManager::COMPONENT_COLLIDABLE | GameObjectManager::COMPONENT_INPUTHANDLER);
    Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
    // use default scale and quaternion value
    mts->add_arg("translation_x", 0.0f).add_arg("translation_y", 0.0f);
    gob_manager_->message_transform(player_ss_id, mts);
    Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
    mrs->add_arg("material", "res/materials/player_ship.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
    gob_manager_->message_renderable(player_ss_id, mrs);
    Message::ShPtr mcs(new Message(Message::COLLIDABLE_SCALE));
    mcs->add_arg("scale_x", 1.732f).add_arg("scale_y", 1.732f).add_arg("scale_z", 10.0f);
    gob_manager_->message_collidable(player_ss_id, mcs);

    (gob_manager_->get_collidable(player_ss_id))->set_callback(boost::bind(&TrilliumScripts::player_ss_update, shared_from_this(), _1));
    (gob_manager_->get_input_handler(player_ss_id))->set_callback(boost::bind(&TrilliumScripts::player_ss_controls, shared_from_this(), _1));
  }

  void player_ss_update(float delta) {
    // derp. Why bother sending messages if we have access to the actual object.
    Transform::ShPtr ss_xf = gob_manager_->get_transform(player_ss_id);
    Vector3f pos = ss_xf->get_position();
    CollidableObject::ShPtr ss_c = gob_manager_->get_collidable(player_ss_id);
    ss_xf->set_translate(pos + (delta * (ss_xf->get_rotation() * ss_c->get_velocity())));

    Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
    mts->add_arg("translation_x", pos.x()).add_arg("translation_y", pos.y()).add_arg("translation_z", 20.0f);
    gob_manager_->message_transform(camera_->get_id(), mts);
  };

  bool player_ss_controls(SDL_Event e) {
    Message::ShPtr mcv(new Message(Message::COLLIDABLE_VELOCITY));
    Vector3f velo;
    bool ret = false;

    switch (e.type) {
      case SDL_KEYDOWN:
        switch(e.key.keysym.sym) {
          case SDLK_w: velo.y() = 10; ret = true; break;
          case SDLK_s: velo.y() = -10; ret = true; break;
        };
        mcv->add_arg("velocity_x", velo.x()).add_arg("velocity_y", velo.y());
        gob_manager_->message_collidable(player_ss_id, mcv);
        break;
      case SDL_KEYUP:
        switch(e.key.keysym.sym) {
          case SDLK_w: velo.y() = 0; ret = true; break;
          case SDLK_s: velo.y() = 0; ret = true; break;
        };
        mcv->add_arg("velocity_x", velo.x()).add_arg("velocity_y", velo.y());
        gob_manager_->message_collidable(player_ss_id, mcv);
        break;
    };

    return ret;
  }

 private:
  GameObjectManager::ShPtr gob_manager_;
  Camera::ShPtr camera_;

  unsigned int player_ss_id;
};

#endif
