#ifndef COSMOS_TEXTUREMANAGER_H_
#define COSMOS_TEXTUREMANAGER_H_

#include <vector>

#include <memory>

#include "Texture.hpp"
#include "util.hpp"

enum class TexFilter { Nearest, Linear };
enum class TexWrap { Repeat, Clamp, Mirror };

class TextureManager {
 public:
	using ShPtr = std::shared_ptr<TextureManager>;

	TextureManager();

  static TextureManager& get();

	// Iterates through the list of loaded textures searching for a texture with the same name as "name".
	// Returns the first texture found with a matching name. Returns an empty pointer if no matching texture
	// is found.
	const Texture::ShPtr get_texture(const std::string name) const;

  // Loads a texture from the given path if it hasn't been loaded yet.
  // Returns the existing texture if already loaded.
  Texture::ShPtr load_texture(const std::string& path,
                              TexFilter filter = TexFilter::Linear,
                              TexWrap wrap = TexWrap::Repeat);

 private:
	bool loaded_;
	std::vector<std::string> tex_names_;
	std::vector<Texture::ShPtr> textures_;
	
	void init();
  
  // Iterates through the list of textures that need to be loaded and loads them. First checks to see if
	// textures have been loaded to prevent duplicate loadings
	void load_textures();
	
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;
};

#endif
