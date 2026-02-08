#include "Particle.hpp"

void Particle::update(float delta, const Vector3f& gravity) {
  if (lifetime_ > 0) {
    lifetime_ -= delta;
    velo_ += delta * gravity;
    pos_ += delta * velo_;
  }
}
