#ifndef COSMOS_TRANSFORM_H_
#define COSMOS_TRANSFORM_H_

#include <memory>
#include <iostream>
#include <string>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Material.hpp"
#include "Matrix4f.hpp"
#include "Mesh.hpp"
#include "Quaternion.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the space transform of an object
 */
class Transform {
 public:
	typedef std::shared_ptr<Transform> ShPtr;

  Transform(unsigned int id): id_(id), scale_(Vector3f(1.0f, 1.0f, 1.0f)) {}
  unsigned int id() const { return id_; }

  // Build and return the model matrix (applies scale, rotation, translation)
  glm::mat4 get_matrix() const;
  // Build and return the inverse model matrix (for camera view)
  glm::mat4 get_inverse_matrix() const;
  // Build and return just the inverse rotation matrix
  glm::mat4 get_rotation_matrix() const;

  // Legacy: apply directly onto GL matrix stack (calls glMultMatrixf)
  void apply();
  void apply_inverse();
  void apply_rotation();

  void set_direction(const Vector3f& dir);

  // S Q T
  Transform& set_scale(const Vector3f& scale) { scale_ = scale; return *this; }
  Transform& set_quat(const Quaternion& quat) { quat_ = quat; return *this; }
  Transform& set_translate(const Vector3f& trans) { translate_ = trans; return *this; }

  Vector3f get_scale() { return scale_; }
  Quaternion get_rotation() { return quat_; }
  Vector3f get_position() { return translate_; }

  Transform& rotate(const Vector3f& axis, float angle);
  Transform& rotate_relative(const Vector3f& axis, float angle);

 private:
  const unsigned int id_;

  Vector3f scale_;
  Quaternion quat_;
  Vector3f translate_;

  // Helper: build a rotation matrix from the internal quaternion
  glm::mat4 quatToMat4(const Quaternion& q) const;

  DISALLOW_COPY_AND_ASSIGN(Transform);
};

#endif
