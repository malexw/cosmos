#include "Transform.hpp"

#include <boost/pointer_cast.hpp>

void Transform::handle_message(Message::ShPtr msg) {

  if (msg->type_ == Message::TRANSFORM_SET) {

    if (msg->has_key("scale_x")) {
      scale_ = Vector3f(msg->get_float("scale_x"), msg->get_float("scale_y"), msg->get_float("scale_z"));
    } else {
      scale_ = Vector3f::ONES;
    }
    if (msg->has_key("quaternion_x")) {
      quat_ = Quaternion(msg->get_float("quaternion_x"), msg->get_float("quaternion_y"), msg->get_float("quaternion_z"), msg->get_float("quaternion_w"));
    } else {
      quat_ = Quaternion();
    }
    if (msg->has_key("translation_x")) {
      translate_ = Vector3f(msg->get_float("translation_x"), msg->get_float("translation_y"), msg->get_float("translation_z"));
    } else {
      translate_ = Vector3f::ZEROS;
    }

  } else if (msg->type_ == Message::TRANSFORM_UPDATE) {

    if (msg->has_key("scale_x")) {
      scale_.x() *= msg->get_float("scale_x");
      scale_.y() *= msg->get_float("scale_y");
      scale_.z() *= msg->get_float("scale_z");
    }
    if (msg->has_key("yaw")) {
      // TODO Need to be able to choose to yaw around local Y
      rotate(Vector3f::NEGATIVE_Y, msg->get_int("yaw"));
    }
    if (msg->has_key("pitch")) {
      rotate_relative(Vector3f::NEGATIVE_X, msg->get_int("pitch"));
    }
    if (msg->has_key("translation_x")) {
      translate_.x() += msg->get_float("translation_x");
      translate_.y() += msg->get_float("translation_y");
      translate_.z() += msg->get_float("translation_z");
    }

  } else if (msg->type_ == Message::TRANSFORM_LOOKAT) {

    set_direction(Vector3f(msg->get_float("direction_x"), msg->get_float("direction_y"), msg->get_float("direction_z")));

  }
}

void Transform::apply() {
  glMultMatrixf(Matrix4f::modelFromSqt(scale_, quat_, translate_).to_array());
}

void Transform::apply_inverse() {
  Vector3f iT = -translate_;
  Vector3f iS(1 / scale_.x(), 1 / scale_.y(), 1 / scale_.z());
  Quaternion iR = quat_.invert();

  iT = iR * iT; // rotate
  iT *= iS; // scale

  glMultMatrixf(Matrix4f::modelFromSqt(iS, iR, iT).to_array());
}

void Transform::apply_rotation() {
  glMultMatrixf(get_rotation_matrix()->to_array());
}

Matrix4f::ShPtr Transform::get_rotation_matrix() {
  Quaternion iR = quat_.invert();

  Matrix4f::ShPtr m(new Matrix4f(Matrix4f::modelFromSqt(Vector3f::ONES, iR, Vector3f::ZEROS)));
  return m;
}

Matrix4f::ShPtr Transform::get_matrix() {
  Matrix4f::ShPtr m(new Matrix4f(Matrix4f::modelFromSqt(scale_, quat_, translate_)));
  return m;
}

Transform& Transform::rotate(const Vector3f& axis, float angle) {
  Quaternion q(axis, angle);
  quat_ = q.normalize() * quat_;
}

Transform& Transform::rotate_relative(const Vector3f& axis, float angle) {
  Quaternion q(quat_ * axis, angle);
  quat_ = q.normalize() * quat_;
}

void Transform::set_direction(const Vector3f& dir) {
  Vector3f ndir = -(dir.normalize());
  Vector3f x = Vector3f::UNIT_Y.cross(ndir).normalize();
  Vector3f y = ndir.cross(x).normalize();

  Quaternion q(x, y, ndir);
  set_quat(q);
}
