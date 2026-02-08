#ifndef COSMOS_PLAYERINPUTHANDLER_H_
#define COSMOS_PLAYERINPUTHANDLER_H_

#include <memory>
//#include <vector>
#include <string>

#include <SDL3/SDL.h>
//#include "SDL/SDL_opengl.h"

#include "CollidableObject.hpp"
#include "CosmosConfig.hpp"
#include "InputHandler.hpp"
#include "Transform.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for GameObjects to handle input
 */
class PlayerInputHandler : public InputHandler {
 public:
	typedef std::shared_ptr<PlayerInputHandler> ShPtr;
  
  PlayerInputHandler(SDL_Window* window, Transform::ShPtr transform, CollidableObject::ShPtr collidable);
  
  virtual void handleInput(SDL_Event e);

 private:
  SDL_Window* window_;
  Vector3f velo_;
  Vector3f rot_;
  CollidableObject::ShPtr collidable_;
  Transform::ShPtr transform_;
  bool grabbing_;
  
  CosmosConfig& config_;
  
  DISALLOW_COPY_AND_ASSIGN(PlayerInputHandler);
};

#endif
