#ifndef COSMOS_ShaderProgram_H_
#define COSMOS_ShaderProgram_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "Shaders/FragmentShader.hpp"
#include "Shaders/VertexShader.hpp"
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

  const bool link() const;

  void run() const { glUseProgram(shader_id_); }
  void setf(std::string varname, float value);
  void seti(std::string varname, int value);

private:
  const std::string name_;
  const unsigned int shader_id_;

  unsigned int vert_count_;
  unsigned int frag_count_;

  void init();

  DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
};

#endif
