#ifndef COSMOS_GEOMETRYSHADER_H_
#define COSMOS_GEOMETRYSHADER_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "FileBlob.hpp"
#include "Renderer.hpp"
#include "util.hpp"

class GeometryShader {
public:
  typedef boost::shared_ptr<GeometryShader> ShPtr;

  GeometryShader(FileBlob::ShPtr file);
  ~GeometryShader();

  const int get_id() const { return shader_id_; }
  const bool compile() const;

private:
  int shader_id_;

  DISALLOW_COPY_AND_ASSIGN(GeometryShader);
};

#endif
