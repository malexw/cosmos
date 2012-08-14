#ifndef COSMOS_INPUTHANDLER_H_
#define COSMOS_INPUTHANDLER_H_

#include <string>

#include <boost/function.hpp>
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

  bool handle_input(SDL_Event e);

  InputHandler& set_callback(boost::function<bool (SDL_Event e)> callback) { event_callback_ = callback; }

 private:
  const unsigned int id_;

  boost::function<bool (SDL_Event e)> event_callback_;

  DISALLOW_COPY_AND_ASSIGN(InputHandler);
};

#endif
