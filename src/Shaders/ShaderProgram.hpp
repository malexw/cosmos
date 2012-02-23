#ifndef COSMOS_ShaderProgram_H_
#define COSMOS_ShaderProgram_H_

#include <string>

#include <boost/shared_ptr.hpp>

//#include "Shaders/FragmentShader.hpp"
//#include "Shaders/GeometryShader.hpp"
//#include "Shaders/VertexShader.hpp"
#include "Shaders/Shader.hpp"
#include "Matrix4f.hpp"
#include "Renderer.hpp"

#include "util.hpp"

class ShaderProgram {
public:
  typedef boost::shared_ptr<ShaderProgram> ShPtr;

  ShaderProgram(std::string name);
  ~ShaderProgram();

  // Returns the name of the ShaderProgram
  std::string get_name() const { return name_; }

  // Compares the name of this ShaderProgram with another name. Returns true if they're equal, false otherwise.
  const bool is_name(const std::string& rhs) const { return name_.compare(rhs) == 0; }

  // Return the id assigned by glCreateShader
  const unsigned int get_id() const { return shader_id_; }

  void attach_shader(FragmentShader::ShPtr frag);
  void attach_shader(VertexShader::ShPtr vert);
  void attach_shader(GeometryShader::ShPtr geom);

  const bool link();

  void run() const { glUseProgram(shader_id_); }
  void setf(std::string varname, float value);
  void set3f(std::string varname, float v1, float v2, float v3);
  void seti(std::string varname, int value);
  void setmat(std::string varname, const Matrix4f& value);
  void set_block_binding(std::string varname, unsigned int binding_point);

  void print_program_log();

private:
  const std::string name_;
  const unsigned int shader_id_;

  unsigned int vert_count_;
  unsigned int frag_count_;
  unsigned int geom_count_;

  void init();

  DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
};

#endif
