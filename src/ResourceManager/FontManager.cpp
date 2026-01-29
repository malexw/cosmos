#include <iostream>
#include <vector>

#include <SDL2/SDL_opengl.h>

#include "FontManager.hpp"

FontManager::FontManager()
  : loaded_(false) {
	init();
}

void FontManager::init() {
	font_names_.push_back(std::string("res/textures/terminal.png"));
  load_fonts();
}

FontManager& FontManager::get() {
  static FontManager instance;
  return instance;
}

/*
 * Build a single VBO containing 256 quads (one per character glyph).
 * Each quad is 6 vertices (2 triangles) with 4 floats per vertex:
 *   x, y, u, v
 * Character i occupies vertices [i*6 .. i*6+5].
 * The quad for character i is 8x8 pixels at the origin; the caller is
 * responsible for translating by (charIndex * 8, 0) when rendering a string.
 */
void FontManager::load_fonts() {
  if (loaded_) {
		std::cout << "FontMan: Error - fonts already loaded" << std::endl;
		return;
	}

  TextureManager& texman = TextureManager::get();

  int font_count = font_names_.size();

  for (int j = 0; j < font_count; ++j) {
		const Texture::ShPtr fonttex = texman.get_texture(font_names_.at(j));

		if (fonttex) {
			// Build vertex data for all 256 characters
			// Each character: 6 vertices × 4 floats (x, y, u, v) = 24 floats
			std::vector<float> verts;
			verts.reserve(256 * 24);

			for (int i = 0; i < 256; ++i) {
				float cy = 1.0f - (float)(i % 16) / 16.0f;
				float cx = 1.0f - (float)(i / 16) / 16.0f;

				int ci = 255 - i;
				// Map character ci to its quad data
				// We store quads at index ci so that ASCII lookup works directly
				// Actually, we'll fill the array linearly and index by character code
				(void)ci; // ci was used for display list indexing; not needed here

				float u0 = cx - 0.0625f;
				float u1 = cx;
				float v0 = cy;
				float v1 = cy - 0.0625f;

				// Triangle 1: bottom-left, bottom-right, top-right
				verts.push_back(0); verts.push_back(0); verts.push_back(u0); verts.push_back(v0);
				verts.push_back(8); verts.push_back(0); verts.push_back(u1); verts.push_back(v0);
				verts.push_back(8); verts.push_back(8); verts.push_back(u1); verts.push_back(v1);
				// Triangle 2: bottom-left, top-right, top-left
				verts.push_back(0); verts.push_back(0); verts.push_back(u0); verts.push_back(v0);
				verts.push_back(8); verts.push_back(8); verts.push_back(u1); verts.push_back(v1);
				verts.push_back(0); verts.push_back(8); verts.push_back(u0); verts.push_back(v1);
			}

			// The display-list code stored quads at index (255 - i) for character i.
			// We need the same mapping: ASCII code c → quad at slot (255 - (255 - c)) = c.
			// But the loop above fills slot i with data computed from (255-i)-th character,
			// which means slot 0 = char 255, slot 1 = char 254, ... slot 255 = char 0.
			// To look up character c, we'd use slot (255 - c).
			// This matches the original display-list scheme.

			GLuint vbo;
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(float), verts.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			Font::ShPtr f(new Font(font_names_.at(j)));
			f->set_vbo(vbo);
			f->set_texture(fonttex);
			fonts_.push_back(f);
		}
	}
}

const Font::ShPtr FontManager::get_font(std::string name) const {
	for (const Font::ShPtr& font : fonts_) {
		if (font->is_name(name)) {
			return font;
		}
	}

  std::cout << "Error: font <" << name << "> not found" << std::endl;
	return Font::ShPtr();
}
