#ifndef COSMOS_FRAGMENTSHADER_H_
#define COSMOS_FRAGMENTSHADER_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "FileBlob.hpp"
#include "Renderer.hpp"
#include "util.hpp"

class FragmentShader {
public:
  typedef boost::shared_ptr<FragmentShader> ShPtr;

  FragmentShader(FileBlob::ShPtr file);
  ~FragmentShader();

  const int get_id() const { return shader_id_; }
  const bool compile() const;

private:
  int shader_id_;

  DISALLOW_COPY_AND_ASSIGN(FragmentShader);
};

#endif
