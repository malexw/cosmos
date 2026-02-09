#include "Timer.hpp"

#include <algorithm>

void Timer::frame_start() {
    last_start_ = frame_start_;
    gettimeofday(&frame_start_, NULL);
}

float Timer::frame_delta() const {
    float dt = static_cast<float>(frame_start_.tv_sec - last_start_.tv_sec)
             + static_cast<float>(frame_start_.tv_usec - last_start_.tv_usec) / 1000000.0f;
    return (dt > PAUSE_THRESHOLD) ? PAUSE_DT : dt;
}
