#include "Shader.hpp"

Shader::Shader(GLenum type, const char** source)
 : shader_id_(glCreateShader(type)) {
  glShaderSource(shader_id_, 1, source, 0);
}

Shader::~Shader() {
  // Shaders aren't deleted until they've been detached from all programs
  glDeleteShader(shader_id_);
}

const bool Shader::compile() const {
  glCompileShader(shader_id_);
  GLint status;
  glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &status);
  bool success = status == GL_TRUE;

  if (!success) {
    print_shader_log();
  }

  return success;
}

void Shader::print_shader_log() const {
  int length = 0;
  int count = 0;

  glGetShaderiv(shader_id_, GL_INFO_LOG_LENGTH, &length);

  if (length > 0) {
    char data[length];
    glGetShaderInfoLog(shader_id_, length, &count, data);
    std::cout << data;
  }
}
