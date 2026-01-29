#include "Texture.hpp"

Texture::Texture(std::string name)
  : name_(name),
    texture_index_(0) { }

std::string Texture::get_name() const {
	return name_;
}

bool Texture::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}

int Texture::get_index() const {
	return texture_index_;
}

int Texture::get_gltype() const {
	return gltype_;
}

int Texture::get_bytecount() const {
	return bytecount_;
}

void Texture::set_index(int index) {
	texture_index_ = index;
}
