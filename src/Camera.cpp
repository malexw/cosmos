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
  // ----------- OLD WAY --------------------------------
  /*float x = quat_.x(); float y = quat_.y(); float z = quat_.z(); float w = quat_.w();
  float dx = x+x; float dy = y+y; float dz = z+z;
  float xy = x*dy; float xz = x*dz; float xw = dx*w; float yz = y*dz; float yw = dy*w; float zw = dz*w;
  float x2 = dx*x; float y2 = dy*y; float z2 = dz*z;

  Vector3f pos(translate_);
  pos = quat_*pos;
  
  //std::cout << "trans" << translate_ << std::endl;
  //std::cout << "pos" << pos << std::endl;

  Matrix4f m( 1.0f-(y2+z2), (xy+zw), (xz-yw), 0.0f,
                (xy-zw), 1.0f-(x2+z2), (yz+xw), 0.0f,
                (xz+yw), (yz-xw), 1.0f-(x2+y2), 0.0f,
                pos.x(), pos.y(), pos.z(), 1.0f );

  m[0] = m[0] * scale_.x(); m[1] = m[1] * scale_.x(); m[2] = m[2] * scale_.x();
  m[4] = m[4] * scale_.y(); m[5] = m[5] * scale_.y(); m[7] = m[7] * scale_.y();
  m[8] = m[8] * scale_.z(); m[9] = m[9] * scale_.z(); m[10] = m[10] * scale_.z();
  
  glMultMatrixf(m.to_array());*/
}

Camera& Camera::set_direction(const Vector3f& dir) {
  Vector3f ndir = -(dir.normalize());
  Vector3f x = Vector3f(0, 1, 0).cross(ndir).normalize();
  Vector3f y = ndir.cross(x).normalize();
  // This works too
  //Vector3f ndir(dir.normalize());
  //ndir.z() = -ndir.z();//(dir.x(), dir.y(), -(dir.z()));
  //Vector3f y = Vector3f(ndir.x(), ndir.z(), -(ndir.y()));
  //Vector3f x = y.cross(ndir);
  // Working
  // For now, only worry about rotations about the x-axis
  //Vector3f ndir(dir.x(), dir.y(), -(dir.z()));
  //Vector3f x(1.0f, 0.0f, 0.0f);
  //Vector3f y = ndir.normalize().cross(x).normalize();

  std::cout << x << y << ndir << std::endl;
  Quaternion q(x, y, ndir);
  std::cout << q << std::endl;
  set_quat(q);
}

Camera& Camera::rotate(const Vector3f& axis, float angle) {
  Quaternion q(quat_ * axis, angle);
  quat_ = q.normalize() * quat_;
}

Camera& Camera::shift(const Vector3f& dir) {
}

void Camera::update(float delta) {
  //std::cout << (delta * velo_) << std::endl;
  
  translate_ += (delta * (quat_ * velo_));
}
