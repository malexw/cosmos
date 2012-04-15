#ifndef COSMOS_SHADER_H_
#define COSMOS_SHADER_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "FileBlob.hpp"
#include "Renderer.hpp"
#include "util.hpp"

class Shader {
 public:
  typedef boost::shared_ptr<Shader> ShPtr;

  Shader(GLenum type, const char** source);
  ~Shader();

  const int get_id() const { return shader_id_; }
  const bool compile() const;
  void print_shader_log() const;

 private:
  int shader_id_;

  DISALLOW_COPY_AND_ASSIGN(Shader);
};

class VertexShader : public Shader {
 public:
  typedef boost::shared_ptr<VertexShader> ShPtr;
  VertexShader(const char** source) : Shader(GL_VERTEX_SHADER, source) {}
 private:
  DISALLOW_COPY_AND_ASSIGN(VertexShader);
};

class GeometryShader : public Shader {
 public:
  typedef boost::shared_ptr<GeometryShader> ShPtr;
  GeometryShader(const char** source) : Shader(GL_GEOMETRY_SHADER, source) {}
 private:
  DISALLOW_COPY_AND_ASSIGN(GeometryShader);
};

class FragmentShader : public Shader {
 public:
  typedef boost::shared_ptr<FragmentShader> ShPtr;
  FragmentShader(const char** source) : Shader(GL_FRAGMENT_SHADER, source) {}
 private:
  DISALLOW_COPY_AND_ASSIGN(FragmentShader);
};

#endif
