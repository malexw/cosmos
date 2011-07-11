#include "Camera.hpp"

void Camera::apply() {

  float x = quat_.x(); float y = quat_.y(); float z = quat_.z(); float w = quat_.w();
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
  
  glMultMatrixf(m.to_array());
}

Camera& Camera::set_direction(const Vector3f& dir) {
  // For now, only worry about rotations about the x-axis
  Vector3f ndir(dir.x(), dir.y(), -(dir.z()));
  Vector3f x(1.0f, 0.0f, 0.0f);
  Vector3f y = ndir.normalize().cross(x).normalize();

  set_quat(Quaternion(x, y, ndir.normalize()));
}

Camera& Camera::shift(const Vector3f& dir) {
}
