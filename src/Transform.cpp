#include "Transform.hpp"

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

/*void Transform::load() {
  glLoadMatrixf(trans_.to_array());
}*/

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
  Vector3f x = Vector3f(0, 1, 0).cross(ndir).normalize();
  Vector3f y = ndir.cross(x).normalize();

  Quaternion q(x, y, ndir);
  set_quat(q);
}

/*Transform& Transform::rotate(const Quaternion& q) {
  float x = q.x(); float y = q.y(); float z = q.z(); float w = q.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;
  Matrix4f rot( 1.0f-(y2-z2), (xy+zw), (xz-yw), 0.0f,
                (xy-zw), 1.0f-(x2-z2), (yz+xw), 0.0f,
                (xz+yw), (yz-xw), 1.0f-(x2-y2), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = rot * trans_;
  itrans_ = itrans_ * rot.transpose();
  
  return *this;*/
  // Following code may not work
  /*float x = q.x(); float y = q.y(); float z = q.z(); float w = q.w();
  float xx = x*x; float yy = y*y; float zz = z*z;
  float xy = x*y; float xz = x*z; float xw = x*w; float yz = y*z; float yw = y*w; float zw = z*w;
  Matrix4f rot( 1-2*(yy-zz), 2*(xy+zw), 2*(xz-yw), 0.0f,
                2*(xy-zw), 1-2*(xx-zz), 2*(yz+xw), 0.0f,
                2*(xz+yw), 2*(yz-xw), 1-2*(xx-yy), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = rot * trans_;
  itrans_ = itrans_ * rot.transpose();
  
  return *this;
}*/

/*
Transform& Transform::rotateX(float angle) {
  double cos_t = cos(angle * M_PI / 180);
  double sin_t = sin(angle * M_PI / 180);
  Matrix4f rot( 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, cos_t, sin_t, 0.0f,
                0.0f, -sin_t, cos_t, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = trans_ * rot;
  itrans_ = itrans_ * rot.transpose();
  
  return *this;
}

Transform& Transform::rotateY(float angle) {
  double cos_t = cos(angle * M_PI / 180);
  double sin_t = sin(angle * M_PI / 180);
  Matrix4f rot( cos_t, 0.0f, -sin_t, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                sin_t, 0.0f, cos_t, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = trans_ * rot;
  itrans_ = itrans_ * rot.transpose();
  
  return *this;
}

Transform& Transform::rotateZ(float angle) {
  double cos_t = cos(angle * M_PI / 180);
  double sin_t = sin(angle * M_PI / 180);
  Matrix4f rot( cos_t, sin_t, 0.0f, 0.0f,
                -sin_t, cos_t, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = trans_ * rot;
  itrans_ = itrans_ * rot.transpose();
  
  return *this;
}

Transform& Transform::scale(const Vector3f& scale) {
  Matrix4f s( scale.x(), 0.0f, 0.0f, 0.0f,
              0.0f, scale.y(), 0.0f, 0.0f,
              0.0f, 0.0f, scale.z(), 0.0f,
              0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = s * trans_;
  
  s[0] = 1/scale.x(); s[5] = 1/scale.y(); s[10] = 1/scale.z();
  itrans_ = s * itrans_;
  
  return *this;
};*/

//Transform& Transform::translate(const Vector3f& translation) {
  /*Matrix4f trans( 1.0f, 0.0f, 0.0f, translation.x(),
                  0.0f, 1.0f, 0.0f, translation.y(),
                  0.0f, 0.0f, 1.0f, translation.z(),
                  0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = trans * trans_;
  
  //trans[12] = translation.x(); trans[13] = translation.y(); trans[14] = translation.z();
  itrans_ = trans * itrans_;
  
  return *this;*/
  /*Matrix4f trans( 1.0f, 0.0f, 0.0f, translation.x(),
                  0.0f, 1.0f, 0.0f, translation.y(),
                  0.0f, 0.0f, 1.0f, translation.z(),
                  0.0f, 0.0f, 0.0f, 1.0f );*/
  /*Matrix4f trans( 1.0f, 0.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 0.0f, 0.0f,
                  0.0f, 0.0f, 1.0f, 0.0f,
                  translation.x(), translation.y(), translation.z(), 1.0f );
  trans_ = trans_ * trans;

  itrans_ = itrans_ * trans;
  
  return *this;
}*/
