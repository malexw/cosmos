#ifndef COSMOS_FONT_H_
#define COSMOS_FONT_H_

#include <string>

#include <memory>

#include <SDL2/SDL_opengl.h>

#include "Texture.hpp"
#include "util.hpp"

class Font {
public:
	typedef std::shared_ptr<Font> ShPtr;

	Font(std::string name);

	std::string get_name() const;
	bool is_name(const std::string& rhs) const;

	GLuint get_vbo() const { return vbo_; }
	void set_vbo(GLuint vbo) { vbo_ = vbo; }

	const Texture::ShPtr get_texture() const;
	void set_texture(Texture::ShPtr tex);

private:
	std::string name_;
	GLuint vbo_;
	Texture::ShPtr texture_;

	DISALLOW_COPY_AND_ASSIGN(Font);
};

#endif
