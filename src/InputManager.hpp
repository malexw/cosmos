#ifndef COSMOS_INPUTMANAGER_H_
#define COSMOS_INPUTMANAGER_H_

#include <memory>
//#include <vector>
#include <list>
#include <string>

#include <SDL2/SDL.h>
//#include "SDL/SDL_opengl.h"

#include "InputHandler.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
//#include "Vector3f.hpp"

/*
 * A class for managing things that can handle input
 */
class InputManager {
 public:
	typedef std::shared_ptr<InputManager> ShPtr;

	InputManager() {}
  
  void handleInput() const;
  void pushHandler(InputHandler::ShPtr handler) { handlers_.push_back(handler); }
  void popHandler() { handlers_.pop_back(); }
  InputHandler::ShPtr getHandler(unsigned int id);

 private:
  std::list<InputHandler::ShPtr> handlers_;
  
  DISALLOW_COPY_AND_ASSIGN(InputManager);
};

#endif
