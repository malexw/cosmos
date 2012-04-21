#ifndef ISABELA_ISABELASCRIPTS_H_
#define ISABELA_ISABELASCRIPTS_H_

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

class IsabelaScripts: public boost::enable_shared_from_this<IsabelaScripts> {
 public:
  typedef boost::shared_ptr<IsabelaScripts> ShPtr;

  IsabelaScripts(GameObjectManager::ShPtr gob_manager, Camera::ShPtr camera)
   : gob_manager_(gob_manager), camera_(camera), camera_velo_x(0.0f), camera_velo_y(0.0f) {
  }

  void init() {
    // Create cameras?

    // Home planets
    {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", -10.0f).add_arg("translation_y", 0.0f).add_arg("translation_z", 0.0f);//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", -10.0f).add_arg("translation_y", 0.0f).add_arg("translation_z", 0.0f).add_arg("scale_x", 1.5f).add_arg("scale_y", 1.5f).add_arg("scale_z", 1.5f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/owned_planet.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    // Friendly neighbours


    {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", 10.0f).add_arg("translation_y", 0.0f).add_arg("translation_z", 0.0f);//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", 10.0f).add_arg("translation_y", 0.0f).add_arg("translation_z", 0.0f).add_arg("scale_x", 1.5f).add_arg("scale_y", 1.5f).add_arg("scale_z", 1.5f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/enemy_planet.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    // Enemy neighbours

    // Create the playfield
    /*for (int i = 0; i < 4; ++i) {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
      float rx = fmod((rand() / static_cast<float>(100)), 20) - 10;
      float ry = fmod((rand() / static_cast<float>(100)), 20) - 10;
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", rx).add_arg("translation_y", ry).add_arg("translation_z", 0.0f);//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }*/

    // Create the player camera
    player_camera_id = gob_manager_->spawn(GameObjectManager::COMPONENT_INPUTHANDLER);
    Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
    mts->add_arg("translation_x", 0.0f).add_arg("translation_y", 0.0f).add_arg("translation_z", 20.0f);
    gob_manager_->message_transform(camera_->get_id(), mts);

    (gob_manager_->get_collidable(camera_->get_id()))->set_callback(boost::bind(&IsabelaScripts::camera_update, shared_from_this(), _1));
    (gob_manager_->get_input_handler(player_camera_id))->set_callback(boost::bind(&IsabelaScripts::camera_controls, shared_from_this(), _1));

  }

  void camera_update(float delta) {
    Message::ShPtr mtu(new Message(Message::TRANSFORM_UPDATE));
    mtu->add_arg("translation_x", camera_velo_x).add_arg("translation_y", camera_velo_y).add_arg("translation_z", 0.0f);
    gob_manager_->message_transform(camera_->get_id(), mtu);
  };

  bool camera_controls(SDL_Event e) {
    bool ret = false;

    switch (e.type) {
      case SDL_KEYDOWN:
        switch(e.key.keysym.sym) {
          case SDLK_w: camera_velo_y = 0.5f; ret = true; break;
          case SDLK_s: camera_velo_y = -0.5f; ret = true; break;
          case SDLK_a: camera_velo_x = -0.5f; ret = true; break;
          case SDLK_d: camera_velo_x = 0.5f; ret = true; break;
        };
        break;
      case SDL_KEYUP:
        switch(e.key.keysym.sym) {
          case SDLK_w: camera_velo_y = 0.0f; ret = true; break;
          case SDLK_s: camera_velo_y = 0.0f; ret = true; break;
          case SDLK_a: camera_velo_x = 0.0f; ret = true; break;
          case SDLK_d: camera_velo_x = 0.0f; ret = true; break;
        };
        break;
    };

    return ret;
  }

 private:
  GameObjectManager::ShPtr gob_manager_;
  Camera::ShPtr camera_;

  unsigned int player_camera_id;
  float camera_velo_x;
  float camera_velo_y;
};

#endif
