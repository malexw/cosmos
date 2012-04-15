#include "Texture.hpp"

Texture::Texture(std::string name)
  : name_(name) {
  glGenTextures(1, &tex_address_);
}

Texture::~Texture() {
  unsigned int arr[] = { tex_address_ };
  glDeleteTextures(1, arr);
}
