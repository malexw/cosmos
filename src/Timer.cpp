#include <iostream>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Timer.hpp"

void Timer::frame_start() {
  if (!paused_) {
    last_start_ = frame_start_;
    gettimeofday(&frame_start_, NULL);
    fps_counter_ += frame_delta();
    frame_counter_ += 1;
    //if (frame_counter_ % 60 == 0) {
    if (fps_counter_ > 10) {
      std::cout << "FPS " << frame_counter_ / 10 << std::endl;
      frame_counter_ = 1;
      fps_counter_ = 0;
    }
  }
}

void Timer::frame_stop() {
  if (!paused_) {
    gettimeofday(&frame_end_, NULL);
  }
}
