#ifndef COSMOS_PLAYERINPUTHANDLER_H_
#define COSMOS_PLAYERINPUTHANDLER_H_

#include <boost/shared_ptr.hpp>
//#include <vector>
#include <string>

#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"

#include "CollidableObject.hpp"
#include "Transform.hpp"
#include "InputHandler.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for GameObjects to handle input
 */
class PlayerInputHandler : public InputHandler {
 public:
	typedef boost::shared_ptr<PlayerInputHandler> ShPtr;
  
  PlayerInputHandler(unsigned int id);
  
  virtual void handleInput(SDL_Event e);

 private:
  Vector3f velo_;
  Vector3f rot_;
  CollidableObject::ShPtr collidable_;
  Transform::ShPtr transform_;
  bool grabbing_;
  
  DISALLOW_COPY_AND_ASSIGN(PlayerInputHandler);
};

#endif
