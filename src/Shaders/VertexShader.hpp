#ifndef COSMOS_VERTEXSHADER_H_
#define COSMOS_VERTEXSHADER_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "FileBlob.hpp"
#include "Renderer.hpp"
#include "util.hpp"

class VertexShader {
public:
  typedef boost::shared_ptr<VertexShader> ShPtr;

  VertexShader(FileBlob::ShPtr file);
  ~VertexShader();

  const int get_id() const { return shader_id_; }
  const bool compile() const;

private:
  int shader_id_;

  DISALLOW_COPY_AND_ASSIGN(VertexShader);
};

#endif
