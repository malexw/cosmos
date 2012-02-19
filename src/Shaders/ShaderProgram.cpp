#include "ShaderProgram.hpp"

ShaderProgram::ShaderProgram(std::string name)
 : name_(name), shader_id_(glCreateProgram()), vert_count_(0), frag_count_(0) {}

ShaderProgram::~ShaderProgram() {
  // Attached shaders are automatically deleted
  glDeleteProgram(shader_id_);
}

void ShaderProgram::attach_shader(VertexShader::ShPtr vert) {
  glAttachShader(shader_id_, vert->get_id());
  vert_count_ += 1;
}

void ShaderProgram::attach_shader(FragmentShader::ShPtr frag) {
  glAttachShader(shader_id_, frag->get_id());
  frag_count_ += 1;
}

void ShaderProgram::attach_shader(GeometryShader::ShPtr geom) {
  glAttachShader(shader_id_, geom->get_id());
  geom_count_ += 1;
}

const bool ShaderProgram::link() const {
  if (vert_count_ == 0 || frag_count_ == 0) {
    if (vert_count_ == 0) {
      std::cout << "Shader program <" << name_ << "> has no vertex shader" << std::endl;
    }
    if (frag_count_ == 0) {
      std::cout << "Shader program <" << name_ << "> has no fragment shader" << std::endl;
    }
    return false;
  }

  glLinkProgram(shader_id_);
  GLint status;
  glGetProgramiv(shader_id_, GL_LINK_STATUS, &status);
  bool success = status == GL_TRUE;

  if (!success) {
    int length = 0;
    int count = 0;

    glGetProgramiv(shader_id_, GL_INFO_LOG_LENGTH, &length);

    if (length > 0) {
      char data[length];
      glGetProgramInfoLog(shader_id_, length, &count, data);
      std::cout << data;
    }
  }

  return success;
}

void ShaderProgram::setf(std::string varname, float value) {
  glUseProgram(shader_id_);
  GLint var_index = glGetUniformLocation(shader_id_, varname.c_str());
  // TODO check for errors here
  glUniform1f(var_index, value);
  glUseProgram(0);
}

void ShaderProgram::set3f(std::string varname, float v1, float v2, float v3) {
  glUseProgram(shader_id_);
  GLint var_index = glGetUniformLocation(shader_id_, varname.c_str());
  glUniform3f(var_index, v1, v2, v3);
  glUseProgram(0);
}

void ShaderProgram::seti(std::string varname, int value) {
  glUseProgram(shader_id_);
  GLint var_index = glGetUniformLocation(shader_id_, varname.c_str());
  // TODO check for errors here
  glUniform1i(var_index, value);
  glUseProgram(0);
}

void ShaderProgram::setmat(std::string varname, const Matrix4f& value) {
  glUseProgram(shader_id_);
  GLint var_index = glGetUniformLocation(shader_id_, varname.c_str());
  glUniformMatrix4fv(var_index, 16, GL_FALSE, value.to_array());
  glUseProgram(0);
}

void ShaderProgram::set_block_binding(std::string varname, unsigned int binding_point) {
  glUseProgram(shader_id_);
  GLuint var_index = GlRenderer::glGetUniformBlockIndex(shader_id_, varname.c_str());
  if (var_index != GL_INVALID_INDEX) {
    GlRenderer::glUniformBlockBinding(shader_id_, var_index, binding_point);
  } else {
    std::cout << "Shader <" << name_ << "> could not bind <" << varname << ">" << std::endl;
  }
  glUseProgram(0);
}
