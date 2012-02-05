#ifndef COSMOS_INPUTHANDLER_H_
#define COSMOS_INPUTHANDLER_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"

#include "util.hpp"

/*
 * A class for GameObjects to handle input
 */
class InputHandler {
 public:
  typedef boost::shared_ptr<InputHandler> ShPtr;
  
  InputHandler(unsigned int id): id_(id) {}
  const unsigned int id() const { return id_; }
  
  virtual void handleInput(SDL_Event e) = 0;

 private:
  const unsigned int id_;
  
  DISALLOW_COPY_AND_ASSIGN(InputHandler);
};

#endif
