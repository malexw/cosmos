#include "InputHandler.hpp"

bool InputHandler::handle_input(SDL_Event e) {
  if (event_callback_) {
    return event_callback_(e);
  } else {
    return false;
  }
}
