#ifndef COSMOS_TEXTUREMANAGER_H_
#define COSMOS_TEXTUREMANAGER_H_

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"
#include "Renderer.hpp"

#include "Texture.hpp"
#include "util.hpp"

class TextureManager {
 public:
  typedef boost::shared_ptr<TextureManager> ShPtr;
  typedef std::map<std::string, Texture::ShPtr> TextureTable;

  TextureManager();

  /*
   * Searches through the list of loaded textures for one with a name of path. Returns it if it was found, otherwise tries
   * to load an image from that path. If loading fails, returns the default texture.
   */
  Texture::ShPtr get_texture(const std::string& name);

 private:
  TextureTable textures_;
  std::vector<std::string> tex_names_;

  void init();

  /*
   * Loads the image pointed at by path onto the GPU
   */
  const bool load_texture(const std::string& path);

  /*
   * Checking the values of the SDL_Surface format appears to be the best way to get the image format.
   */
  const GLenum get_surface_format(const SDL_Surface* const surface);

  DISALLOW_COPY_AND_ASSIGN(TextureManager);
};

#endif
