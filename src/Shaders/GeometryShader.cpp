#include "GeometryShader.hpp"

GeometryShader::GeometryShader(FileBlob::ShPtr file)
 : shader_id_(glCreateShader(GL_GEOMETRY_SHADER)) {

  const char* src = file->get_bytes();
  glShaderSource(shader_id_, 1, &src, 0);
}

GeometryShader::~GeometryShader() {
  // Shaders aren't deleted until they've been detached from all programs
  glDeleteShader(shader_id_);
}

const bool GeometryShader::compile() const {
  glCompileShader(shader_id_);
  GLint status;
  glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &status);
  bool success = status == GL_TRUE;

  if (!success) {
    int length = 0;
    int count = 0;

    glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &length);

    if (length > 0) {
      char data[length];
      glGetShaderInfoLog(shader_id_, length, &count, data);
      std::cout << data;
    }
  }

  return success;
}
