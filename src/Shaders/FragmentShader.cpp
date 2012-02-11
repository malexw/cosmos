#include "FragmentShader.hpp"

FragmentShader::FragmentShader(FileBlob::ShPtr file)
 : shader_id_(glCreateShader(GL_FRAGMENT_SHADER)) {

  const char* src = file->get_bytes();
  glShaderSource(shader_id_, 1, &src, 0);
}

FragmentShader::~FragmentShader() {
  // Shaders don't actually get deleted until they have been detached from their program
  glDeleteShader(shader_id_);
}

const bool FragmentShader::compile() const {
  glCompileShader(shader_id_);
  GLint status;
  glGetShaderiv(shader_id_, GL_COMPILE_STATUS, &status);
  bool success = status == GL_TRUE;

  /*if (!success) {
    int length = 0;
    int count = 0;

    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

    if (length > 0) {
      char data[length];
      glGetShaderInfoLog(id, length, &count, data);
      std::cout << data;
    }
  }*/

  return success;
}
