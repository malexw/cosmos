#ifndef COSMOS_INPUTHANDLER_H_
#define COSMOS_INPUTHANDLER_H_

#include <boost/shared_ptr.hpp>
//#include <vector>
#include <string>

#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"

#include "util.hpp"
//#include "Vector2f.hpp"
//#include "Vector3f.hpp"

/*
 * A class for GameObjects to handle input
 */
class InputHandler {
 public:
	typedef boost::shared_ptr<InputHandler> ShPtr;
  
  //InputHandler(GameObject::ShPtr parent): parent_(parent) id_(parent.id()) {}
	InputHandler(unsigned int id): id_(id) {}
  const unsigned int id() const { return id_; }
  
  virtual void handleInput(SDL_Event e) = 0;

 private:
  //GameObject::ShPtr parent_;
  const unsigned int id_;
  
  DISALLOW_COPY_AND_ASSIGN(InputHandler);
};

#endif
