#include "ParticleEmitter.hpp"

void ParticleEmitter::update(float delta) {
  foreach (Particle::ShPtr part, parts_) {
    part->update(delta);
  }
  
  int new_part_count = delta / generation_rate_;
  generation_remainder_ -= fmod(delta, generation_rate_);
  if (generation_remainder_ < 0) {
    generation_remainder_ += generation_rate_;
    new_part_count += 1;
  }
  
  for (int i = 0; i < new_part_count; ++i) {
    next_part_ = (next_part_ + i + 1) % part_count_;
    //std::cout << "next part " << next_part_ << std::endl;
    float angle = (fmod(rand(), (radius_ * 2000)) / 1000.0f) - radius_;
    float dir = (rand() % 180000) / 1000.0f;
    Quaternion deflection(up_.cross(norm_), angle);
    Quaternion rotation(norm_, dir);
    //std::cout << "Rand " << angle << std::endl;
    parts_.at(next_part_)->reset(pos_, speed_*(rotation*(deflection*norm_)), lifetime_);
  }

  // calculate the roation matrix
  //Matrix4f(1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1);
  // for each particle
  //translate_ += (delta * (quat_ * velo_));
}

void ParticleEmitter::render(Transform::ShPtr cam) {

  glPushMatrix();

  foreach (Particle::ShPtr part, parts_) {
    part->render(cam);
  }
  
  // restores the modelview matrix
  glPopMatrix();
}

void ParticleEmitter::rotate(const Vector3f& axis, float angle) {
  Quaternion q(axis, angle);
  norm_ = q * norm_;
  up_ = q * up_;
}

void ParticleEmitter::init() {
  for (int i = part_count_; i > 0; --i) {
    Particle::ShPtr p(new Particle(renderable_));
    parts_.push_back(p);
  }
}
