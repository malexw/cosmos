#include "ParticleEmitter.hpp"

#include "Mesh.hpp"

ParticleEmitter::ParticleEmitter(Transform::ShPtr transform, Renderable::ShPtr renderable,
  float speed, float lifetime, float radius, float generation_rate)
 : transform_(transform), renderable_(renderable),
  speed_(speed), lifetime_(lifetime), radius_(radius),
  part_count_((generation_rate*lifetime)),
  generation_rate_(1/generation_rate),
  generation_remainder_(1/generation_rate),
  next_part_(0) {

  init();
}

void ParticleEmitter::init() {
  mesh_.reset(new Mesh("particle-effect", GL_POINTS));

  // Just using billboarded quads for now. Will update once we need to do something more interesting
  Vector3f pos = transform_->get_position();

  for (int i = part_count_; i > 0; --i) {
    mesh_->add_point(pos);
    // Also fill in the initial particle data
    particle_velocities_.push_back(Vector3f(0.0f, 0.0f, 0.0f));
    particle_lifetimes_.push_back(generation_rate_*i);
  }
  mesh_->upload_to_gpu_type(GL_DYNAMIC_DRAW);

  renderable_->set_mesh(mesh_);
}

void ParticleEmitter::update(float delta) {
  for (int i = part_count_; i > 0; --i) {
    particle_lifetimes_[i] -= delta;

    if (particle_lifetimes_[i] <= 0) {
      float angle = (fmod(rand(), (radius_ * 2000)) / 1000.0f) - radius_;
      float dir = (rand() % 180000) / 1000.0f;
      Quaternion root_rotation = transform_->get_rotation();
      Vector3f root_x = root_rotation * Vector3f::UNIT_X;
      Vector3f root_y = root_rotation * Vector3f::UNIT_Y;
      Quaternion deflection(root_x, angle);
      Quaternion rotation(root_y, dir);
      particle_velocities_[i] = speed_*(rotation*(deflection*root_y));

      // respawning
      Vector3f pos = transform_->get_position();
      pos = pos + ((delta + particle_lifetimes_[i]) * particle_velocities_[i]);
      Vector2f uv_tl(0.0f, 0.0f);
      mesh_->deform(i, pos, uv_tl, Vector3f::ZEROS);

      particle_lifetimes_[i] += lifetime_;
    } else {
      Vector3f translation = delta * particle_velocities_[i];
      mesh_->deform_relative(i, translation, Vector2f(0.0f, 0.0f), Vector3f::ZEROS);
    }
  }

  mesh_->update_on_gpu();
}

void ParticleEmitter::render(Transform::ShPtr cam) {
  renderable_->render();
}
