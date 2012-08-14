#include "Camera.hpp"

Camera::Camera(GameObjectManager::ShPtr gob_man, MatrixStack::ShPtr matrix_stack)
 : gob_man_(gob_man), matrix_stack_(matrix_stack) {
  // Still not sure if we should be constructing the camera components in the GobManager.
  // Or if the camera should even use the same components as GameObjects
  id_ = gob_man_->spawn(GameObjectManager::COMPONENT_TRANSFORM | GameObjectManager::COMPONENT_COLLIDABLE);
  transform_ = gob_man_->get_transform(id_);
  collidable_ = gob_man_->get_collidable(id_);
}

void Camera::upload_matrices(unsigned int binding) const {

  matrix_stack_->upload_matrices(binding);

  //Vector3f iT = -(transform_->get_position());
  //Vector3f scale = transform_->get_scale();
  //Vector3f iS(1 / scale.x(), 1 / scale.y(), 1 / scale.z());
  //Quaternion iR = (transform_->get_rotation()).invert();

  //iT = iR * iT;
  //iT *= iS;

  //glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  //glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat)*16, Matrix4f::modelFromSqt(iS, iR, iT).to_array());
  //glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16, sizeof(GLfloat)*16, projection_matrix_.to_array());
  //glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // TODO consider moving this to initialization and subclassing Camera
  //glBindBufferBase(GL_UNIFORM_BUFFER, binding, matrix_buffer_);
}

void Camera::upload_model_matrix(const Matrix4f& model_matrix) const {
  matrix_stack_->upload_model_matrix(model_matrix);
  /*Vector3f iT = -(transform_->get_position());
  Vector3f scale = transform_->get_scale();
  Vector3f iS(1 / scale.x(), 1 / scale.y(), 1 / scale.z());
  Quaternion iR = (transform_->get_rotation()).invert();

  iT = iR * iT;
  iT *= iS;

  Matrix4f model_view = model_matrix * Matrix4f::modelFromSqt(iS, iR, iT);
  Matrix4f model_view_projection = model_view * projection_matrix_;

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*3, sizeof(GLfloat)*16, model_matrix.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*4, sizeof(GLfloat)*16, model_view.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*5, sizeof(GLfloat)*16, model_view_projection.to_array());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
}

// TODO Maybe the World should handle uploading these matrices with a special shader
void Camera::upload_skybox_matrix(const Matrix4f& model_matrix) const {
  matrix_stack_->upload_skybox_matrix(model_matrix);
  /*Quaternion iR = (transform_->get_rotation()).invert();

  Matrix4f model_view = model_matrix * Matrix4f::modelFromSqt(Vector3f::ONES, iR, Vector3f::ZEROS);
  Matrix4f model_view_projection = model_view * Matrix4f::projectionPerspectiveMatrix(45, static_cast<float>(960.0f) / static_cast<float>(600.0f), 0.1, 3);

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*3, sizeof(GLfloat)*16, model_matrix.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*4, sizeof(GLfloat)*16, model_view.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*5, sizeof(GLfloat)*16, model_view_projection.to_array());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
}

// TODO Use a special shader for drawing this full-screen quad
void Camera::upload_imposter_matrix() const {
  matrix_stack_->upload_imposter_matrix();
  /*Matrix4f model_matrix = Matrix4f::IDENTITY;
  float ASPECT_RATIO = static_cast<float>(960) / static_cast<float>(600);
  Matrix4f model_view = Matrix4f::modelFromSqt(Vector3f(ASPECT_RATIO, 1, 1), Quaternion(), Vector3f::ZEROS);
  Matrix4f model_view_projection = model_view * Matrix4f::projectionOrthoMatrix(-ASPECT_RATIO/2, ASPECT_RATIO/2, -0.5, 0.5, -1, 1);

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*3, sizeof(GLfloat)*16, model_matrix.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*4, sizeof(GLfloat)*16, model_view.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*5, sizeof(GLfloat)*16, model_view_projection.to_array());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);*/
}
