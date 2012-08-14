#include <boost/pointer_cast.hpp>

#include "GameObjectManager.hpp"
#include "CollidableObject.hpp"

const unsigned int CollidableObject::TYPE_SPHERE = 0;
const unsigned int CollidableObject::TYPE_CAPSULE = 1;

CollidableObject::CollidableObject(unsigned int id, unsigned int type, Transform::ShPtr transform)
 : id_(id), type_(type), scale_(Vector3f(1.0f, 1.0f, 1.0f)), transform_(transform) {
  quadric_ = gluNewQuadric();
  gluQuadricNormals(quadric_, GLU_SMOOTH);
}

void CollidableObject::handle_message(Message::ShPtr msg) {
  if (msg->type_ == Message::COLLIDABLE_SCALE) {
    scale_ = Vector3f(msg->get_float("scale_x"), msg->get_float("scale_y"), msg->get_float("scale_z"));
  } else if (msg->type_ == Message::COLLIDABLE_VELOCITY) {
    velo_ = Vector3f(msg->get_float("velocity_x"), msg->get_float("velocity_y"), msg->get_float("velocity_z"));
  }
}

void CollidableObject::update(float delta) {
  if (update_callback_) {
    update_callback_(delta);
  } else {
    //transform_->set_translate(transform_->get_position() + (delta * (transform_->get_rotation() * velo_)));
  }
}

void CollidableObject::check(CollidableObject::ShPtr rhs) {
  Vector3f distance;
  float length, actual_distance;
  Vector3f rhs_size = rhs->get_scale();
  switch (rhs->get_type()) {
    case CollidableObject::TYPE_CAPSULE: {
      // Check against the first sphere
      Vector3f sphere1_pos = rhs->get_transform()->get_position() + Vector3f(0, 0, scale_.z()/2);//(rhs->get_transform()->get_rotation() * Vector3f(0, 0, scale_.z()/2));
      distance = (transform_->get_position()+Vector3f(0, 0, scale_.z()/2)) - sphere1_pos;
      length = distance.lengthSquare();
      //std::cout << "Distance: " << length << std::endl;
      actual_distance = (scale_.x() + rhs_size.x()) * (scale_.x() + rhs_size.x());

      if (actual_distance > length) {
        float correction = sqrt(actual_distance) - sqrt(length);
        distance = correction * distance.normalize();
        //std::cout << "correction: " << distance << std::endl;
        transform_->set_translate(distance + transform_->get_position());
      }

      Vector3f sphere2_pos = rhs->get_transform()->get_position() - Vector3f(0, 0, scale_.z()/2);//(rhs->get_transform()->get_rotation() * Vector3f(0, 0, -(scale_.z()/2)));
      distance = transform_->get_position() - sphere2_pos;
      length = distance.lengthSquare();
      //std::cout << "Distance: " << length << std::endl;
      actual_distance = (scale_.x() + rhs_size.x()) * (scale_.x() + rhs_size.x());

      if (actual_distance > length) {
        float correction = sqrt(actual_distance) - sqrt(length);
        distance = correction * distance.normalize();
        //std::cout << "correction: " << distance << std::endl;
        transform_->set_translate(distance + transform_->get_position());
      }

      // Check against the second sphere
      break; }
    case CollidableObject::TYPE_SPHERE:
      distance = transform_->get_position() - rhs->get_transform()->get_position();
      length = distance.lengthSquare();
      actual_distance = (scale_.x() + rhs_size.x()) * (scale_.x() + rhs_size.x());

      if (actual_distance > length) {
        float correction = sqrt(actual_distance) - sqrt(length);
        distance = correction * distance.normalize();
        transform_->set_translate(distance + transform_->get_position());
      }
      break;
  }
}

/*void CollidableObject::gjk(CollidableObject::ShPtr rhs) {
  Vector3f search_direction = (transform_->get_position() - rhs->get_transform()->get_position()).normalize();
  simplex_[0] = supportSS(rhs, search_direction);

  search_direction = -search_direction;
  simplex_[1] = supportSS(rhs, search_direction);

  if (simplex_[1].dot(search_direction) <= 0) {
    //std::cout << "Fail 1" << std::endl;
    return;
  }

  //std::cout << "Pass 1" << std::endl;
  Vector3f edge1 = simplex_[1] - simplex_[0];
  search_direction = edge1.cross(-simplex_[1]).cross(edge1);
  simplex_[2] = supportSS(rhs, search_direction);

  if (simplex_[2].dot(search_direction) <= 0) {
    //std::cout << "Fail 2" << std::endl;
    return;
  }

  Vector3f edge2 = simplex_[2] - simplex_[1];
  search_direction = edge2.cross(-simplex_[2]).cross(edge2);

  if (search_direction.dot(simplex_[2]) <= 0) {
    //std::cout << "Fail 3" << std::endl;
    return;
  }
  Vector3f edge3 = simplex_[2] - simplex_[0];
  if (edge3.cross(edge2).cross(edge3).dot > 0) {
    return;
  }

  simplex_[3] = supportSS(rhs, search_direction);

  if (simplex_[3].dot(search_direction) > 0) {
    return;
  }
  Vector3f edge4 = simplex_[3] - simplex_[2];
  search_direction = edge4.cross(-simplex_[3]).cross(edge4);

  if (search_direction.dot(simplex_[3]) > 0) {
    return;
  }

  std::cout << "Hit" << std::endl;
}*/

void CollidableObject::render_collision() {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);
  // Cylinder
  switch (type_) {
    case CollidableObject::TYPE_CAPSULE:
      glPushMatrix();
      glTranslatef(0.0f, 0.0f, -(scale_.z()/2));
      gluCylinder(quadric_, scale_.x(), scale_.x(), scale_.z(), 16, 16);
      glTranslatef(0.0f, 0.0f, scale_.z());
      gluSphere(quadric_, scale_.x(), 16, 16);
      glTranslatef(0.0f, 0.0f, -(scale_.z()));
      gluSphere(quadric_, scale_.x(), 16, 16);
      glPopMatrix();
      break;
    case CollidableObject::TYPE_SPHERE:
      gluSphere(quadric_, scale_.x(), 16, 16);
      break;
  }
  glEnable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/*Vector3f CollidableObject::supportSS(CollidableObject::ShPtr rhs, const Vector3f direction) {
  Vector3f lhspoint = transform_->get_position() + (scale_.x() * direction);
  Vector3f rhspoint = rhs->get_transform()->get_position() + (rhs->get_scale().x() * (-direction));
  //std::cout << lhspoint << " " << rhspoint << std::endl;
  return lhspoint - rhspoint;
}*/

bool CollidableObject::query(const Ray& ray) {
  Vector3f pos = (transform_->get_position()) - ray.position();
  Vector3f dir = ray.direction();
  float dc = (dir.dot(pos));
  return ((dc*dc) - pos.lengthSquare() + (scale_.x() * scale_.x())) >= 0;
}
