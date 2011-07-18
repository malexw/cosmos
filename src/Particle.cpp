#include "Particle.hpp"

void Particle::update(float delta) {
  if (lifetime_ > 0) {
    lifetime_ -= delta;
    pos_ += delta * velo_;
  }
}

void Particle::render(Transform::ShPtr cam) {
  if (lifetime_ > 0) {
    glPushMatrix();
    Vector3f up = cam->get_rotation() * Vector3f::UNIT_Y;
    Vector3f right = cam->get_rotation() * Vector3f::UNIT_X;
    Vector3f front = right.cross(front);
    //glMultMatrixf(Matrix4f(1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  pos_.x(), pos_.y(), pos_.z(), 1).to_array());
    glMultMatrixf(Matrix4f(right.x(), right.y(), right.z(), 0,  up.x(), up.y(), up.z(), 0,  front.x(), front.y(), front.z(), 0,  pos_.x(), pos_.y(), pos_.z(), 1).to_array());
    renderable_->render();
    glPopMatrix();
  }
}
