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
  //particle_velocities_(part_count_),
  //particle_lifetimes_(part_count_) {
    
  init();
}

void ParticleEmitter::init() {
  mesh_.reset(new Mesh("particle-effect"));
  
  // Just using billboarded quads for now. Will update once we need to do something more interesting
  Vector3f pos = transform_->get_position();
  Vector2f uv_tl(0.0f, 0.0f);
  Vector2f uv_tr(1.0f, 0.0f);
  Vector2f uv_bl(0.0f, 1.0f);
  Vector2f uv_br(1.0f, 1.0f);
  // Borrow the data area for the normal to store our opacity since we don't need the normal
  Vector3f norm = Vector3f::ZEROS;
  Vector3f color = Vector3f::ZEROS;
  for (int i = part_count_; i > 0; --i) {
    mesh_->add_triangle(pos, uv_tl, norm, color, pos, uv_bl, norm, color, pos, uv_tr, norm, color);
    mesh_->add_triangle(pos, uv_tr, norm, color, pos, uv_bl, norm, color, pos, uv_br, norm, color);
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
      Vector2f uv_tl(0.0f, 0.0f);
      Vector2f uv_tr(1.0f, 0.0f);
      Vector2f uv_bl(0.0f, 1.0f);
      Vector2f uv_br(1.0f, 1.0f);
      pos = pos + ((delta + particle_lifetimes_[i]) * particle_velocities_[i]);
      unsigned int v_index = i*6;
      mesh_->deform(v_index, pos + Vector3f(-0.5, 0.5, 0), uv_tl, Vector3f::ZEROS);
      mesh_->deform(v_index+1, pos + Vector3f(-0.5, -0.5, 0), uv_bl, Vector3f::ZEROS);
      mesh_->deform(v_index+2, pos + Vector3f(0.5, 0.5, 0), uv_tr, Vector3f::ZEROS);
      mesh_->deform(v_index+3, pos + Vector3f(0.5, 0.5, 0), uv_tr, Vector3f::ZEROS);
      mesh_->deform(v_index+4, pos + Vector3f(-0.5, -0.5, 0), uv_bl, Vector3f::ZEROS);
      mesh_->deform(v_index+5, pos + Vector3f(0.5, -0.5, 0), uv_br, Vector3f::ZEROS);
      
      particle_lifetimes_[i] += lifetime_;
    } else {
      Vector3f translation = delta * particle_velocities_[i];
      unsigned int v_index = i*6;
      mesh_->deform_relative(v_index, translation, Vector2f(0.0f, 0.0f), Vector3f::ZEROS);
      mesh_->deform_relative(v_index+1, translation, Vector2f(0.0f, 0.0f), Vector3f::ZEROS);
      mesh_->deform_relative(v_index+2, translation, Vector2f(0.0f, 0.0f), Vector3f::ZEROS);
      mesh_->deform_relative(v_index+3, translation, Vector2f(0.0f, 0.0f), Vector3f::ZEROS);
      mesh_->deform_relative(v_index+4, translation, Vector2f(0.0f, 0.0f), Vector3f::ZEROS);
      mesh_->deform_relative(v_index+5, translation, Vector2f(0.0f, 0.0f), Vector3f::ZEROS);
    }
  }
  
  mesh_->update_on_gpu();
}

void ParticleEmitter::render(Transform::ShPtr cam) {

  glPushMatrix();
  //Vector3f pos_ = transform_->get_position();
  Vector3f pos_ = Vector3f::ZEROS;
  Vector3f up = cam->get_rotation() * Vector3f::UNIT_Y;
  Vector3f right = cam->get_rotation() * Vector3f::UNIT_X;
  Vector3f front = right.cross(front);
  glMultMatrixf(Matrix4f(1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  pos_.x(), pos_.y(), pos_.z(), 1).to_array());
  //glMultMatrixf(Matrix4f(1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  pos_.x(), pos_.y(), pos_.z(), 1).to_array());
  //glMultMatrixf(Matrix4f(right.x(), right.y(), right.z(), 0,  up.x(), up.y(), up.z(), 0,  front.x(), front.y(), front.z(), 0,  pos_.x(), pos_.y(), pos_.z(), 1).to_array());
  renderable_->render();
  glPopMatrix();

  /*glPushMatrix();

  foreach (Particle::ShPtr part, parts_) {
    part->render(cam);
  }
  
  // restores the modelview matrix
  glPopMatrix();*/
}
