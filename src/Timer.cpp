#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Timer.hpp"

void Timer::frame_start() {
  if (!paused_) {
    last_start_ = frame_start_;
    gettimeofday(&frame_start_, NULL);
  }
}

void Timer::frame_stop() {
  if (!paused_) {
    gettimeofday(&frame_end_, NULL);
  }
}
