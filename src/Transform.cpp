#include "Transform.hpp"

glm::mat4 Transform::quatToMat4(const Quaternion& q) const {
  float x = q.x(), y = q.y(), z = q.z(), w = q.w();
  float dx = x+x, dy = y+y, dz = z+z;
  float xy = x*dy, xz = x*dz, xw = dx*w, yz = y*dz, yw = dy*w, zw = dz*w;
  float x2 = dx*x, y2 = dy*y, z2 = dz*z;

  // Column-major (GLM default)
  return glm::mat4(
    1.0f-(y2+z2), (xy+zw),      (xz-yw),      0.0f,
    (xy-zw),      1.0f-(x2+z2), (yz+xw),      0.0f,
    (xz+yw),      (yz-xw),      1.0f-(x2+y2), 0.0f,
    0.0f,         0.0f,         0.0f,          1.0f
  );
}

glm::mat4 Transform::get_matrix() const {
  glm::mat4 rot = quatToMat4(quat_);

  // Apply scale to rotation columns
  glm::mat4 m = rot;
  m[0] *= scale_.x();
  m[1] *= scale_.y();
  m[2] *= scale_.z();

  // Set translation
  m[3] = glm::vec4(translate_.x(), translate_.y(), translate_.z(), 1.0f);

  return m;
}

glm::mat4 Transform::get_inverse_matrix() const {
  Vector3f iT = -translate_;
  Vector3f iS(1.0f / scale_.x(), 1.0f / scale_.y(), 1.0f / scale_.z());
  Quaternion iR = quat_.invert();

  iT = iR * iT;
  iT *= iS;

  glm::mat4 rot = quatToMat4(iR);

  glm::mat4 m = rot;
  m[0] *= iS.x();
  m[1] *= iS.y();
  m[2] *= iS.z();

  m[3] = glm::vec4(iT.x(), iT.y(), iT.z(), 1.0f);

  return m;
}

glm::mat4 Transform::get_rotation_matrix() const {
  Quaternion iR = quat_.invert();
  return quatToMat4(iR);
}

Transform& Transform::rotate(const Vector3f& axis, float angle) {
  Quaternion q(axis, angle);
  quat_ = q.normalize() * quat_;
  return *this;
}

Transform& Transform::rotate_relative(const Vector3f& axis, float angle) {
  Quaternion q(quat_ * axis, angle);
  quat_ = q.normalize() * quat_;
  return *this;
}

void Transform::set_direction(const Vector3f& dir) {
  Vector3f ndir = -(dir.normalize());
  Vector3f x = Vector3f::UNIT_Y.cross(ndir).normalize();
  Vector3f y = ndir.cross(x).normalize();

  Quaternion q(x, y, ndir);
  set_quat(q);
}
