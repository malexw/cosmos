#include "GameObjectManager.hpp"
#include "CollidableObject.hpp"

CollidableObject::CollidableObject(unsigned int id)
 : id_(id), scale_(Vector3f(1.0f, 1.0f, 1.0f)) {
  transform_ = GameObjectManager::get().get_object(id)->get_transform();
}

void CollidableObject::update(float delta) {
  transform_->set_translate(transform_->get_position() + (delta * (transform_->get_rotation() * velo_)));
}

void CollidableObject::check(CollidableObject::ShPtr rhs) {
  Vector3f distance = transform_->get_position() - rhs->get_transform()->get_position();
  float length = distance.lengthSquare();
  Vector3f rhs_size = rhs->get_scale();
  float actual_distance = (scale_.x() + rhs_size.x()) * (scale_.x() + rhs_size.x());
  
  //std::cout << "Dist " << length << " lpos: " << transform_->get_position() << " rpos: " << rhs->get_transform()->get_position() << std::endl;
  if (actual_distance > length) {
    float correction = sqrt(actual_distance) - sqrt(length);
    //std::cout << "correction: " << correction << std::endl;
    
    distance = correction * distance.normalize();
    transform_->set_translate(distance + transform_->get_position());
  }
}

void CollidableObject::render_collision() {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  GLUquadricObj* quadratic_ = gluNewQuadric();
  gluQuadricNormals(quadratic_, GLU_SMOOTH);
  //gluQuadricTexture(quadratic_, GL_TRUE);
  glDisable(GL_CULL_FACE);
  //glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_LIGHTING);
  gluSphere(quadratic_,scale_.x(),16,16);
  glEnable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
