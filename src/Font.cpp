#include "Font.hpp"

Font::Font(std::string name)
  : name_(name),
    vbo_(0) { }

std::string Font::get_name() const {
	return name_;
}

bool Font::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}

const Texture::ShPtr Font::get_texture() const {
	return texture_;
}

void Font::set_texture(Texture::ShPtr tex) {
	texture_ = tex;
}
