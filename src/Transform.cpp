#include "Transform.hpp"

void Transform::apply() {
  glMultMatrixf(trans_.to_array());
}

void Transform::load() {
  glLoadMatrixf(trans_.to_array());
}

Transform& Transform::rotateX(float angle) {
  double cos_t = cos(angle * M_PI / 180);
  double sin_t = sin(angle * M_PI / 180);
  Matrix4f rot( 1.0f, 0.0f, 0.0f, 0.0f,
                0.0f, cos_t, sin_t, 0.0f,
                0.0f, -sin_t, cos_t, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = rot * trans_;
  itrans_ = rot.transpose() * itrans_;
  
  return *this;
}

Transform& Transform::rotateY(float angle) {
  double cos_t = cos(angle * M_PI / 180);
  double sin_t = sin(angle * M_PI / 180);
  Matrix4f rot( cos_t, 0.0f, -sin_t, 0.0f,
                0.0f, 1.0f, 0.0f, 0.0f,
                sin_t, 0.0f, cos_t, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = rot * trans_;
  itrans_ = rot.transpose() * itrans_;
  
  return *this;
}

Transform& Transform::rotateZ(float angle) {
  double cos_t = cos(angle * M_PI / 180);
  double sin_t = sin(angle * M_PI / 180);
  Matrix4f rot( cos_t, sin_t, 0.0f, 0.0f,
                -sin_t, cos_t, 0.0f, 0.0f,
                0.0f, 0.0f, 1.0f, 0.0f,
                0.0f, 0.0f, 0.0f, 1.0f );
  trans_ = rot * trans_;
  itrans_ = rot.transpose() * itrans_;
  
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
};

Transform& Transform::translate(const Vector3f& translation) {
  /*Matrix4f trans( 1.0f, 0.0f, 0.0f, translation.x(),
                  0.0f, 1.0f, 0.0f, translation.y(),
                  0.0f, 0.0f, 1.0f, translation.z(),
                  0.0f, 0.0f, 0.0f, 1.0f );*/
  Matrix4f trans( 1.0f, 0.0f, 0.0f, 0.0f,
                  0.0f, 1.0f, 0.0f, 0.0f,
                  0.0f, 0.0f, 1.0f, 0.0f,
                  translation.x(), translation.y(), translation.z(), 1.0f );
  trans_ = trans * trans_;
  
  trans[3] = translation.x(); trans[7] = translation.y(); trans[11] = translation.z();
  itrans_ = trans * itrans_;
  
  return *this;
}
