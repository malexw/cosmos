#ifndef COSMOS_TEXTURE_H_
#define COSMOS_TEXTURE_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "Renderer.hpp"
#include "util.hpp"

class Texture {
public:
  typedef boost::shared_ptr<Texture> ShPtr;

  Texture(std::string name);
  ~Texture();

  // Returns the name of the texture
  const std::string get_name() const { return name_; }

  // Return the index, AKA "OpenGL Name" of the texture.
  const int get_index() const { return tex_address_; }

private:
  std::string name_;
  unsigned int tex_address_;

  DISALLOW_COPY_AND_ASSIGN(Texture);
};

#endif
