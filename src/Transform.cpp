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

  float x = quat_.x(); float y = quat_.y(); float z = quat_.z(); float w = quat_.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;

  Matrix4f m( 1.0f-(y2+z2), (xy+zw), (xz-yw), 0.0f,
                (xy-zw), 1.0f-(x2+z2), (yz+xw), 0.0f,
                (xz+yw), (yz-xw), 1.0f-(x2+y2), 0.0f,
                translate_.x(), translate_.y(), translate_.z(), 1.0f );

  m[0] = m[0] * scale_.x(); m[1] = m[1] * scale_.x(); m[2] = m[2] * scale_.x();
  m[4] = m[4] * scale_.y(); m[5] = m[5] * scale_.y(); m[7] = m[7] * scale_.y();
  m[8] = m[8] * scale_.z(); m[9] = m[9] * scale_.z(); m[10] = m[10] * scale_.z();

  glMultMatrixf(m.to_array());
}

void Transform::apply_inverse() {
  Vector3f iT = -translate_;
  Vector3f iS(1 / scale_.x(), 1 / scale_.y(), 1 / scale_.z());
  Quaternion iR = quat_.invert();

  iT = iR * iT; // rotate
  iT *= iS; // scale

  float x = iR.x(); float y = iR.y(); float z = iR.z(); float w = iR.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;

  Matrix4f m( 1.0f-(y2+z2), (xy+zw), (xz-yw), 0.0f,
                (xy-zw), 1.0f-(x2+z2), (yz+xw), 0.0f,
                (xz+yw), (yz-xw), 1.0f-(x2+y2), 0.0f,
                iT.x(), iT.y(), iT.z(), 1.0f );

  m[0] = m[0] * iS.x(); m[1] = m[1] * iS.x(); m[2] = m[2] * iS.x();
  m[4] = m[4] * iS.y(); m[5] = m[5] * iS.y(); m[7] = m[7] * iS.y();
  m[8] = m[8] * iS.z(); m[9] = m[9] * iS.z(); m[10] = m[10] * iS.z();

  glMultMatrixf(m.to_array());
}

void Transform::apply_rotation() {
  glMultMatrixf(get_rotation_matrix()->to_array());
}

Matrix4f::ShPtr Transform::get_rotation_matrix() {
  Quaternion iR = quat_.invert();

  float x = iR.x(); float y = iR.y(); float z = iR.z(); float w = iR.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;

  Matrix4f::ShPtr m(new Matrix4f( 1.0f-(y2+z2), (xy+zw), (xz-yw), 0.0f,
              (xy-zw), 1.0f-(x2+z2), (yz+xw), 0.0f,
              (xz+yw), (yz-xw), 1.0f-(x2+y2), 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f ));
  
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
