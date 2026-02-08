#ifndef COSMOS_PARTICLE_H_
#define COSMOS_PARTICLE_H_

#include <memory>

#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A fancy hat
 */
class Particle {
 public:
	typedef std::shared_ptr<Particle> ShPtr;

	Particle()
   : pos_(), velo_(), lifetime_(0.0f), initial_lifetime_(1.0f) {}

  void update(float delta, const Vector3f& gravity);

  void reset(Vector3f position, Vector3f velocity, float lifetime) {
    pos_ = position;
    velo_ = velocity;
    lifetime_ = lifetime;
    initial_lifetime_ = lifetime;
  }

  bool is_alive() const { return lifetime_ > 0.0f; }
  const Vector3f& position() const { return pos_; }

  // Returns 0 at birth, 1 at death
  float age_normalized() const {
    if (initial_lifetime_ <= 0.0f) return 1.0f;
    float t = 1.0f - (lifetime_ / initial_lifetime_);
    if (t < 0.0f) return 0.0f;
    if (t > 1.0f) return 1.0f;
    return t;
  }

 private:
  Vector3f pos_; // position in world coordinates
  Vector3f velo_;
  float lifetime_; // lifetime in seconds
  float initial_lifetime_;

  DISALLOW_COPY_AND_ASSIGN(Particle);
};

#endif
