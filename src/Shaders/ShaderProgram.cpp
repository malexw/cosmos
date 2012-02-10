#include "ShaderProgram.hpp"

const std::string ShaderProgram::get_name() const {
  return name_;
}

const bool ShaderProgram::is_name(const std::string& rhs) const {
  return name_.compare(rhs) == 0;
}

void ShaderProgram::setf(std::string varname, float value) {
  glUseProgram(shader_id_);
  GLint var_index = glGetUniformLocation(shader_id_, varname.c_str());
  // TODO check for errors here
  glUniform1f(var_index, value);
  glUseProgram(0);
}

void ShaderProgram::seti(std::string varname, int value) {
  glUseProgram(shader_id_);
  GLint var_index = glGetUniformLocation(shader_id_, varname.c_str());
  // TODO check for errors here
  glUniform1i(var_index, value);
  glUseProgram(0);
}
