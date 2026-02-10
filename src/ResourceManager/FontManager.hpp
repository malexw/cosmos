#ifndef COSMOS_FONTMANAGER_H_
#define COSMOS_FONTMANAGER_H_

#include <vector>

#include <memory>

#include "Font.hpp"
#include "TextureManager.hpp"
#include "util.hpp"

class FontManager {
 public:
	using ShPtr = std::shared_ptr<FontManager>;

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
	
	FontManager(const FontManager&) = delete;
	FontManager& operator=(const FontManager&) = delete;
};

#endif
