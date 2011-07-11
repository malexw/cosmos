#ifndef COSMOS_PLAYERINPUTHANDLER_H_
#define COSMOS_PLAYERINPUTHANDLER_H_

#include <boost/shared_ptr.hpp>
//#include <vector>
#include <string>

#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"

#include "Camera.hpp"
#include "InputHandler.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
//#include "Vector3f.hpp"

/*
 * A class for GameObjects to handle input
 */
class PlayerInputHandler : public InputHandler {
 public:
	typedef boost::shared_ptr<PlayerInputHandler> ShPtr;
  
  PlayerInputHandler(unsigned int id): InputHandler(id), pos_(Vector3f(0.0f, 10.0f, 20.0f)) {}
  
  void listener(Camera::ShPtr cam) { listener_ = cam; }
  virtual void handleInput(SDL_Event e);

 private:
  Vector3f pos_;
  Camera::ShPtr listener_;
  
  DISALLOW_COPY_AND_ASSIGN(PlayerInputHandler);
};

#endif
