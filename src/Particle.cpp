#include "Particle.hpp"

#include <SDL2/SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager/ShaderManager.hpp"

void Particle::update(float delta) {
  if (lifetime_ > 0) {
    lifetime_ -= delta;
    pos_ += delta * velo_;
  }
}

void Particle::render(Transform::ShPtr cam, const glm::mat4& projView) {
  if (lifetime_ > 0) {
    Vector3f up = cam->get_rotation() * Vector3f::UNIT_Y;
    Vector3f right = cam->get_rotation() * Vector3f::UNIT_X;
    Vector3f front = right.cross(up);
    glm::mat4 billboard(
      right.x(), right.y(), right.z(), 0.0f,
      up.x(),    up.y(),    up.z(),    0.0f,
      front.x(), front.y(), front.z(), 0.0f,
      pos_.x(),  pos_.y(),  pos_.z(),  1.0f
    );
    glm::mat4 mvp = projView * billboard;

    auto unlitProg = ShaderManager::get().get_shader_program("unlit");
    unlitProg->run();
    unlitProg->setMat4("mvp", mvp);

    renderable_->render();
  }
}
