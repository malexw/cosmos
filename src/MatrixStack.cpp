#include "MatrixStack.hpp"

MatrixStack::MatrixStack() {
  glGenBuffers(1, &matrix_buffer_);

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(GLfloat) * 16 * 6, 0, GL_STREAM_DRAW);

  // TODO This matrix is for a static shadow at (5, 15, 5) looking at (5, 0, -30)
  Matrix4f bias(0.5, 0, 0, 0, 0, 0.5, 0, 0, 0, 0, 0.5, 0, 0.5, 0.5, 0.5, 1.0);
  Matrix4f shadow_matrix = Matrix4f::viewFromPositionDirection(Vector3f(5, 15, 5), Vector3f(0, -15, -35)) * (Matrix4f::projectionPerspectiveMatrix(45, 1, 10, 40000) * bias);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*2, sizeof(GLfloat)*16, shadow_matrix.to_array());

  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MatrixStack::upload_matrices(unsigned int binding) const {

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(GLfloat)*16, view_matrix_.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16, sizeof(GLfloat)*16, projection_matrix_.to_array());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  // TODO consider moving this to initialization and subclassing MatrixStack
  glBindBufferBase(GL_UNIFORM_BUFFER, binding, matrix_buffer_);
}

void MatrixStack::upload_model_matrix(const Matrix4f& model_matrix) const {

  Matrix4f model_view = model_matrix * view_matrix_;
  Matrix4f model_view_projection = model_view * projection_matrix_;

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*3, sizeof(GLfloat)*16, model_matrix.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*4, sizeof(GLfloat)*16, model_view.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*5, sizeof(GLfloat)*16, model_view_projection.to_array());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// TODO Maybe the World should handle uploading these matrices with a special shader
void MatrixStack::upload_skybox_matrix(const Matrix4f& model_matrix) const {

  Matrix4f model_view = model_matrix * skybox_view_matrix_;
  Matrix4f model_view_projection = model_view * Matrix4f::projectionPerspectiveMatrix(45, static_cast<float>(960.0f) / static_cast<float>(600.0f), 0.1, 3);

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*3, sizeof(GLfloat)*16, model_matrix.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*4, sizeof(GLfloat)*16, model_view.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*5, sizeof(GLfloat)*16, model_view_projection.to_array());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

// TODO Use a special shader for drawing this full-screen quad
void MatrixStack::upload_imposter_matrix() const {
  Matrix4f model_matrix = Matrix4f::IDENTITY;
  //Matrix4f model_view = Matrix4f::modelFromSqt(Vector3f(2.7, 1.7, 1.0), Quaternion(), Vector3f(0, 0, -2));
  //Matrix4f model_view_projection = model_view * projection_matrix_;
  float ASPECT_RATIO = static_cast<float>(960) / static_cast<float>(600);
  Matrix4f model_view = Matrix4f::modelFromSqt(Vector3f(ASPECT_RATIO, 1, 1), Quaternion(), Vector3f::ZEROS);
  Matrix4f model_view_projection = model_view * Matrix4f::projectionOrthoMatrix(-ASPECT_RATIO/2, ASPECT_RATIO/2, -0.5, 0.5, -1, 1);

  glBindBuffer(GL_UNIFORM_BUFFER, matrix_buffer_);
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*3, sizeof(GLfloat)*16, model_matrix.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*4, sizeof(GLfloat)*16, model_view.to_array());
  glBufferSubData(GL_UNIFORM_BUFFER, sizeof(GLfloat)*16*5, sizeof(GLfloat)*16, model_view_projection.to_array());
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void MatrixStack::set_view(Transform::ShPtr camera_transform) {
  Vector3f iT = -(camera_transform->get_position());
  Vector3f scale = camera_transform->get_scale();
  Vector3f iS(1 / scale.x(), 1 / scale.y(), 1 / scale.z());
  Quaternion iR = (camera_transform->get_rotation()).invert();

  iT = iR * iT;
  iT *= iS;

  view_matrix_ = Matrix4f::modelFromSqt(iS, iR, iT);
  skybox_view_matrix_ = Matrix4f::modelFromSqt(Vector3f::ONES, iR, Vector3f::ZEROS);
}
