#ifndef COSMOS_CAMERA_H_
#define COSMOS_CAMERA_H_

#include <boost/shared_ptr.hpp>
#include <iostream>

#include "Renderer.hpp"

#include "CollidableObject.hpp"
#include "GameObjectManager.hpp"
#include "Matrix4f.hpp"
#include "MatrixStack.hpp"
#include "Quaternion.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the space Camera of an object
 */
class Camera {
 public:
  typedef boost::shared_ptr<Camera> ShPtr;

  Camera(GameObjectManager::ShPtr gob_man, MatrixStack::ShPtr matrix_stack);

  // These are essentially aliases to make the code look cleaner
  //void apply() const { transform_->apply(); }
  //void apply_inverse() const { transform_->apply_inverse(); }
  //void apply_rotation() const { transform_->apply_rotation(); }

  const unsigned int get_id() const { return id_; }
  Transform::ShPtr get_transform() { return transform_; }
  Matrix4f::ShPtr get_rotation_matrix() { return transform_->get_rotation_matrix(); }
  Matrix4f get_view_projection_matrix() { return matrix_stack_->get_view_projection_matrix(); }

  void upload_matrices(unsigned int binding) const;
  void upload_model_matrix(const Matrix4f& model_matrix) const;
  void upload_skybox_matrix(const Matrix4f& model_matrix) const;
  void upload_imposter_matrix() const;
  void set_projection(Matrix4f projection_matrix) { matrix_stack_->set_projection(projection_matrix); }

 private:
  unsigned int id_;

  GameObjectManager::ShPtr gob_man_;
  Transform::ShPtr transform_;
  CollidableObject::ShPtr collidable_;

  MatrixStack::ShPtr matrix_stack_;

  Matrix4f projection_matrix_;
  GLuint matrix_buffer_;

  DISALLOW_COPY_AND_ASSIGN(Camera);
};

#endif
