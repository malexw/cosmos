#ifndef COSMOS_TIMER_HPP_
#define COSMOS_TIMER_HPP_

#include <sys/time.h>

#include <boost/shared_ptr.hpp>

#include "util.hpp"

class Timer {
 public:
  typedef boost::shared_ptr<Timer> ShPtr;
  
  Timer(): paused_(false) { gettimeofday(&frame_start_, NULL); }

  void frame_start();
  void frame_stop();
  void pause() { paused_ = true; }
  void unpause() { paused_ = false; }
  bool is_paused() { return paused_; }

  float frame_length() { return (frame_end_.tv_sec - frame_start_.tv_sec) + (frame_end_.tv_usec - frame_start_.tv_usec) / 1000000.0f; }
  float frame_delta() { return (frame_start_.tv_sec - last_start_.tv_sec) + (frame_start_.tv_usec - last_start_.tv_usec) / 1000000.0f; }

 private:
  timeval frame_start_, frame_end_;
  timeval last_start_;
  bool paused_;
  
  DISALLOW_COPY_AND_ASSIGN(Timer);
};

#endif
