#include "Camera.hpp"

void Camera::apply() {

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

void Camera::apply_rotation() {
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

/*void Camera::apply_rotation_inv() {
  float x = quat_.x(); float y = quat_.y(); float z = quat_.z(); float w = quat_.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;
  
  Matrix4f m( 1.0f-(y2+z2), (xy+zw), (xz-yw), 0.0f,
                (xy-zw), 1.0f-(x2+z2), (yz+xw), 0.0f,
                (xz+yw), (yz-xw), 1.0f-(x2+y2), 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );

  glMultMatrixf(m.to_array());
}*/

Camera& Camera::set_direction(const Vector3f& dir) {
  Vector3f ndir = -(dir.normalize());
  Vector3f x = Vector3f(0, 1, 0).cross(ndir).normalize();
  Vector3f y = ndir.cross(x).normalize();

  //std::cout << x << y << ndir << std::endl;
  Quaternion q(x, y, ndir);
  //std::cout << q << std::endl;
  set_quat(q);
}

Camera& Camera::rotate(const Vector3f& axis, float angle) {
  Quaternion q(axis, angle);
  quat_ = q.normalize() * quat_;
}

Camera& Camera::rotate_relative(const Vector3f& axis, float angle) {
  Quaternion q(quat_ * axis, angle);
  quat_ = q.normalize() * quat_;
}

Camera& Camera::shift(const Vector3f& dir) {
}

void Camera::update(float delta) {
  //std::cout << (delta * velo_) << std::endl;
  
  translate_ += (delta * (quat_ * velo_));
}

Matrix4f Camera::matrixFromPositionDirection(Vector3f position, Vector3f direction) {
  Vector3f ndir = -(direction.normalize());
  Vector3f vx = Vector3f(0, 1, 0).cross(ndir).normalize();
  Vector3f vy = ndir.cross(vx).normalize();
  Quaternion q(vx, vy, ndir);
  
  Vector3f iT = -position;
  Quaternion iR = q.invert();

  iT = iR * iT; // rotate

  float x = iR.x(); float y = iR.y(); float z = iR.z(); float w = iR.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;
  
  Matrix4f m( 1.0f-(y2+z2), (xy+zw), (xz-yw), 0.0f,
              (xy-zw), 1.0f-(x2+z2), (yz+xw), 0.0f,
              (xz+yw), (yz-xw), 1.0f-(x2+y2), 0.0f,
              iT.x(), iT.y(), iT.z(), 1.0f );
              
  return m;
}
