#ifndef COSMOS_TEXTUREMANAGER_H_
#define COSMOS_TEXTUREMANAGER_H_

#include <vector>

#include <memory>

#include "Texture.hpp"
#include "util.hpp"

class TextureManager {
 public:
	typedef std::shared_ptr<TextureManager> ShPtr;

	TextureManager();
	
  static TextureManager& get();
  
	// Iterates through the list of loaded textures searching for a texture with the same name as "name".
	// Returns the first texture found with a matching name. Returns an empty pointer if no matching texture
	// is found.
	const Texture::ShPtr get_texture(const std::string name) const;

 private:
	bool loaded_;
	std::vector<std::string> tex_names_;
	std::vector<Texture::ShPtr> textures_;
	
	void init();
  
  // Iterates through the list of textures that need to be loaded and loads them. First checks to see if
	// textures have been loaded to prevent duplicate loadings
	void load_textures();
	
	DISALLOW_COPY_AND_ASSIGN(TextureManager);
};

#endif
