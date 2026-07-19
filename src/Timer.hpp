#ifndef COSMOS_TIMER_HPP_
#define COSMOS_TIMER_HPP_

#include <chrono>

#include "util.hpp"

class Timer {
 public:
  static constexpr float PAUSE_THRESHOLD = 1.0f / 10.0f;
  static constexpr float PAUSE_DT = 1.0f / 30.0f;

  Timer() : frame_start_(std::chrono::steady_clock::now()), last_start_(frame_start_) {}

  void frame_start();
  float frame_delta() const;

 private:
  std::chrono::steady_clock::time_point frame_start_;
  std::chrono::steady_clock::time_point last_start_;

  Timer(const Timer&) = delete;
  Timer& operator=(const Timer&) = delete;
};

#endif
