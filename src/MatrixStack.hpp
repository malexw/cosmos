#ifndef COSMOS_MATRIXSTACK_H_
#define COSMOS_MATRIXSTACK_H_

#include <boost/shared_ptr.hpp>
#include <iostream>

#include "Renderer.hpp"

#include "Matrix4f.hpp"
#include "Quaternion.hpp"
#include "Transform.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for managing matrices
 */
class MatrixStack {
 public:
  typedef boost::shared_ptr<MatrixStack> ShPtr;

  MatrixStack();

  void upload_matrices(unsigned int binding) const;
  void upload_model_matrix(const Matrix4f& model_matrix) const;
  void upload_skybox_matrix(const Matrix4f& model_matrix) const;
  void upload_imposter_matrix() const;

  void set_projection(Matrix4f projection_matrix) { projection_matrix_ = projection_matrix; }
  void set_view(Transform::ShPtr camera_transform);
  Matrix4f get_view_projection_matrix() const { return view_matrix_ * projection_matrix_; }

 private:

  Matrix4f projection_matrix_;
  Matrix4f view_matrix_;
  Matrix4f skybox_view_matrix_;
  GLuint matrix_buffer_;

  DISALLOW_COPY_AND_ASSIGN(MatrixStack);
};

#endif
