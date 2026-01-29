#include "Particle.hpp"

#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    glm::mat4 billboard(
      right.x(), right.y(), right.z(), 0.0f,
      up.x(),    up.y(),    up.z(),    0.0f,
      front.x(), front.y(), front.z(), 0.0f,
      pos_.x(),  pos_.y(),  pos_.z(),  1.0f
    );
    glMultMatrixf(glm::value_ptr(billboard));
    renderable_->render();
    glPopMatrix();
  }
}
