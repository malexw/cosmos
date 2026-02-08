#include "Texture.hpp"

Texture::Texture(std::string name)
  : name_(name),
    texture_index_(0),
    default_exposure_(1.0f) { }

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

float Texture::get_default_exposure() const {
	return default_exposure_;
}

void Texture::set_default_exposure(float exposure) {
	default_exposure_ = exposure;
}
