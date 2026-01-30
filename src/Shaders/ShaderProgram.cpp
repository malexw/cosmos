#include "ShaderProgram.hpp"

#include <glm/gtc/type_ptr.hpp>

std::string ShaderProgram::get_name() const {
	return name_;
}

bool ShaderProgram::is_name(const std::string& rhs) const {
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

void ShaderProgram::setMat4(const char* name, const glm::mat4& mat) const {
  GLint loc = glGetUniformLocation(shader_id_, name);
  if (loc >= 0) glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}

void ShaderProgram::setMat3(const char* name, const glm::mat3& mat) const {
  GLint loc = glGetUniformLocation(shader_id_, name);
  if (loc >= 0) glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(mat));
}
