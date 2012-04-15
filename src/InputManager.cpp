#include <iostream>

#include "InputManager.hpp"

void InputManager::handleInput() const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    handlers_.back()->handle_input(e);
  }
}

InputHandler::ShPtr InputManager::getHandler(unsigned int id) {
  foreach (InputHandler::ShPtr handler, handlers_) {
    if (handler->id() == id) {
      return handler;
    }
  }

  std::cout << "ERROR: InputHandler not found" << std::endl;
  return InputHandler::ShPtr();
}
