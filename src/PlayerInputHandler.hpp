#ifndef COSMOS_PLAYERINPUTHANDLER_H_
#define COSMOS_PLAYERINPUTHANDLER_H_

#include <boost/shared_ptr.hpp>
//#include <vector>
#include <string>

#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"

#include "CosmosConfig.hpp"
#include "GameObjectManager.hpp"
#include "InputHandler.hpp"
#include "Message.hpp"
#include "Shaders/ShaderProgram.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for GameObjects to handle input
 */
class PlayerInputHandler : public InputHandler {
 public:
  typedef boost::shared_ptr<PlayerInputHandler> ShPtr;

  PlayerInputHandler(GameObjectManager::ShPtr gob_man, unsigned int id);

  virtual void handleInput(SDL_Event e);

 private:
  Vector3f velo_;
  Vector3f rot_;
  unsigned int camera_id_;
  GameObjectManager::ShPtr gob_man_;
  bool grabbing_;
  int heading_, attitude_;

  ShaderProgram::ShPtr hdr_program_;
  CosmosConfig& config_;

  DISALLOW_COPY_AND_ASSIGN(PlayerInputHandler);
};

#endif
