#ifndef ISABELA_ISABELASCRIPTS_H_
#define ISABELA_ISABELASCRIPTS_H_

#include <iostream>

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/function.hpp>

#include "SDL/SDL.h"

#include "GameObjectManager.hpp"
#include "Message.hpp"
#include "Ray.hpp"
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
    /*unsigned int n_count = 3;
    for (int i = 0; i < n_count; ++i) {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
      float angle = fmod(rand(), 180/n_count) + ((180/n_count)*i);
      float distance = fmod(rand(), 4) + 3;
      std::cout << "Angle " << angle << " dist " << distance << std::endl;
      Vector3f offset = Quaternion(Vector3f::NEGATIVE_Z, angle) * (distance * Vector3f::UNIT_Y);
      offset = offset + Vector3f(-10, 0, 0);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", offset.x()).add_arg("translation_y", offset.y()).add_arg("translation_z", offset.z());//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }*/
    for (int i = 0; i < 3; ++i) {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE | GameObjectManager::COMPONENT_COLLIDABLE);
      Vector3f pos(-6.66, 5 - (i*5), 0);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", pos.x()).add_arg("translation_y", pos.y()).add_arg("translation_z", pos.z());//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    for (int i = 0; i < 5; ++i) {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE | GameObjectManager::COMPONENT_COLLIDABLE);
      Vector3f pos(-3.33, 6.66 - (i*3.33), 0);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", pos.x()).add_arg("translation_y", pos.y()).add_arg("translation_z", pos.z());//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    for (int i = 0; i < 9; ++i) {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE | GameObjectManager::COMPONENT_COLLIDABLE);
      Vector3f pos(0, 8.0 - (i*2), 0);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", pos.x()).add_arg("translation_y", pos.y()).add_arg("translation_z", pos.z());//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    for (int i = 0; i < 5; ++i) {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE | GameObjectManager::COMPONENT_COLLIDABLE);
      Vector3f pos(3.33, 6.66 - (i*3.33), 0);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", pos.x()).add_arg("translation_y", pos.y()).add_arg("translation_z", pos.z());//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }
    for (int i = 0; i < 3; ++i) {
      unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE | GameObjectManager::COMPONENT_COLLIDABLE);
      Vector3f pos(6.66, 5 - (i*5), 0);
      Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
      mts->add_arg("translation_x", pos.x()).add_arg("translation_y", pos.y()).add_arg("translation_z", pos.z());//.add_arg("scale_x", 0.25f).add_arg("scale_y", 0.25f).add_arg("scale_z", 0.25f);
      gob_manager_->message_transform(id, mts);
      Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
      mrs->add_arg("material", "res/materials/asteroid.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
      gob_manager_->message_renderable(id, mrs);
    }

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
      case SDL_MOUSEBUTTONUP:
        if (e.button.button == SDL_BUTTON_LEFT) {
          Vector3f mouse_pos((static_cast<float>(e.button.x)*2/960.0f) - 1, 1 - (static_cast<float>(e.button.y)*2/600.0f), -1);
          mouse_pos = camera_->get_view_projection_matrix().invert() * mouse_pos;
          Vector3f cam_pos = camera_->get_transform()->get_position();
          Ray r(cam_pos, (mouse_pos-cam_pos).normalize());
          //std::cout << r.z_intercept() << std::endl;
          std::list<unsigned int> results = gob_manager_->query_collidables(r);
          foreach (unsigned int id, results) {
            Vector3f pos = gob_manager_->get_transform(id)->get_position();
            if (pos != Vector3f(0, 0, 20)) {
              unsigned int id = gob_manager_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_RENDERABLE);
              Message::ShPtr mts(new Message(Message::TRANSFORM_SET));
              mts->add_arg("translation_x", pos.x()).add_arg("translation_y", pos.y()).add_arg("translation_z", 0.0f).add_arg("scale_x", 1.5f).add_arg("scale_y", 1.5f).add_arg("scale_z", 1.5f);
              gob_manager_->message_transform(id, mts);
              Message::ShPtr mrs(new Message(Message::RENDERABLE_SET));
              mrs->add_arg("material", "res/materials/owned_planet.mtl").add_arg("mesh", "res/meshes/face-center-quad.obj");
              gob_manager_->message_renderable(id, mrs);
            }
          }
        }
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
