#include "Transform.hpp"

#include <boost/pointer_cast.hpp>

void Transform::handle_message(Message::ShPtr msg) {

  if (msg->type() == MESSAGE_TRANSFORM_SET) {
    TransformSetMessage::ShPtr m = boost::static_pointer_cast<TransformSetMessage>(msg);
    scale_ = m->scale;
    quat_ = m->quaternion;
    translate_ = m->translation;
  } else if (msg->type() == MESSAGE_TRANSFORM_UPDATE) {
    TransformUpdateMessage::ShPtr m = boost::static_pointer_cast<TransformUpdateMessage>(msg);
    // TODO Need to multiply the components, not add
    scale_ += m->scale;
    quat_ = (m->local_rotation * m->global_rotation) * quat_;
    translate_ += m->translation;
  } else if (msg->type() == MESSAGE_TRANSFORM_LOOKAT) {
    TransformLookatMessage::ShPtr m = boost::static_pointer_cast<TransformLookatMessage>(msg);
    set_direction(m->direction);
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
  Quaternion iR = quat_.invert();

  float x = iR.x(); float y = iR.y(); float z = iR.z(); float w = iR.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;

  Matrix4f m( 1.0f-(y2+z2), (xy+zw), (xz-yw), 0.0f,
                (xy-zw), 1.0f-(x2+z2), (yz+xw), 0.0f,
                (xz+yw), (yz-xw), 1.0f-(x2+y2), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );

  glMultMatrixf(m.to_array());
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
