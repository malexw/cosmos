#include "Timer.hpp"

#include <algorithm>

void Timer::frame_start() {
    last_start_ = frame_start_;
    frame_start_ = std::chrono::steady_clock::now();
}

float Timer::frame_delta() const {
    float dt = std::chrono::duration<float>(frame_start_ - last_start_).count();
    return (dt > PAUSE_THRESHOLD) ? PAUSE_DT : dt;
}
