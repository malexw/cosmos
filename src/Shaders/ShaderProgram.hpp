#ifndef COSMOS_ShaderProgram_H_
#define COSMOS_ShaderProgram_H_

#include <string>

#include <boost/shared_ptr.hpp>

//#include "SDL/SDL_opengl.h"
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"

#include "util.hpp"

class ShaderProgram {
public:
  typedef boost::shared_ptr<ShaderProgram> ShPtr;

  ShaderProgram(std::string name, int id): name_(name), shader_id_(id) {}

  // Returns the name of the ShaderProgram
  const std::string get_name() const;

  // Compares the name of this ShaderProgram with another name. Returns true if they're equal, false otherwise.
  const bool is_name(const std::string& rhs) const;

  const int get_id() const { return shader_id_; }

  void run() const { glUseProgram(shader_id_); }
  void setf(std::string varname, float value);
  void seti(std::string varname, int value);

private:
  std::string name_;
  int shader_id_;

  DISALLOW_COPY_AND_ASSIGN(ShaderProgram);
};

#endif
