#ifndef COSMOS_FONTMANAGER_H_
#define COSMOS_FONTMANAGER_H_

#include <vector>

#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Font.hpp"
#include "TextureManager.hpp"
#include "util.hpp"

class FontManager {
 public:
	typedef boost::shared_ptr<FontManager> ShPtr;

	FontManager();
	
  static FontManager& get();
	
	// Iterates through the list of loaded textures searching for a texture with the same name as "name".
	// Returns the first texture found with a matching name. Returns an empty pointer if no matching texture
	// is found.
	const Font::ShPtr get_font(const std::string name) const;

 private:
	bool loaded_;
	std::vector<std::string> font_names_;
	std::vector<Font::ShPtr> fonts_;
	
	void init();
  
  // Iterates through the list of textures that need to be loaded and loads them. First checks to see if
	// textures have been loaded to prevent duplicate loadings
	void load_fonts();
	
	DISALLOW_COPY_AND_ASSIGN(FontManager);
};

#endif
