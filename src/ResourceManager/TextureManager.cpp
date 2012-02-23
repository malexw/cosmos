#include <iostream>

#include "SDL/SDL_image.h"

//#include "rgbe.h"

#include "TextureManager.hpp"

TextureManager::TextureManager() {
  init();
}

void TextureManager::init() {

  // Generate the default texture to return when a lookup fails - a 4 square red and white checkerboard pattern
  unsigned int d_width = 64;
  unsigned int d_width_bytes = d_width * 3;
  unsigned int d_width_half = d_width_bytes / 2;
  unsigned int d_height = d_width;
  unsigned int d_height_half = d_height / 2;
  unsigned char d_image[d_height][d_width*3];

  for (int y = d_height-1; y >= d_height_half; --y) {
    for (int x = (d_width_bytes)-1; x >= d_width_half; x -= 3) {
      d_image[y][x] = 0x00; // Blue
      d_image[y][x-1] = 0x00; // Green
      d_image[y][x-2] = 0xFF; // Red
    }
    for (int x = (d_width_half)-1; x >= 0; x -= 3) {
      d_image[y][x] = 0xFF; // Blue
      d_image[y][x-1] = 0xFF; // Green
      d_image[y][x-2] = 0xFF; // Red
    }
  }
  for (int y = (d_height_half)-1; y >= 0; --y) {
    for (int x = (d_width_bytes)-1; x >= d_width_half; x -= 3) {
      d_image[y][x] = 0xFF; // Blue
      d_image[y][x-1] = 0xFF; // Green
      d_image[y][x-2] = 0xFF; // Red
    }
    for (int x = (d_width_half)-1; x >= 0; x -= 3) {
      d_image[y][x] = 0x00; // Blue
      d_image[y][x-1] = 0x00; // Green
      d_image[y][x-2] = 0xFF; // Red
    }
  }

  Texture::ShPtr default_texture (new Texture("__err_texture"));

  glBindTexture(GL_TEXTURE_2D, default_texture->get_index());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, d_width, d_height, 0, GL_RGB, GL_UNSIGNED_BYTE, d_image);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  textures_.insert(TextureTable::value_type(default_texture->get_name(), default_texture));

  // Generate a texture as a target for the shadow mapping FBO
  Texture::ShPtr shadow (new Texture("shadow_map"));

  glBindTexture(GL_TEXTURE_2D, shadow->get_index());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

  textures_.insert(TextureTable::value_type(shadow->get_name(), shadow));

  // Generate a texture as a target for the HDR rendering FBO
  Texture::ShPtr hdr_target(new Texture("hdr target"));

  glBindTexture(GL_TEXTURE_2D, hdr_target->get_index());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 960, 600, 0, GL_RGBA, GL_FLOAT, 0);

  textures_.insert(TextureTable::value_type(hdr_target->get_name(), hdr_target));

  glBindTexture(GL_TEXTURE_2D, 0);
}

const bool TextureManager::load_texture(const std::string& path) {
  bool loaded = false;
  SDL_Surface* image = IMG_Load(path.c_str());

  if (image) {
    GLenum format = get_surface_format(image);
    unsigned int tex_addr;
    Texture::ShPtr tex (new Texture(path));

    glBindTexture(GL_TEXTURE_2D, tex->get_index());
    glTexImage2D(GL_TEXTURE_2D, 0, format, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    textures_.insert(TextureTable::value_type(path, tex));
    loaded = true;
  } else {
    std::cout << "Error: could not load image <" << path << ">" << std::endl;
  }

  return loaded;

  // Loading an RGBE HDR image:
  /*int image_width, image_height;
  FILE* f = fopen(tex_names_[i].c_str(),"rb");
  RGBE_ReadHeader(f,&image_width,&image_height,NULL);
  float* image = (float *)malloc(sizeof(float)*3*image_width*image_height);
  RGBE_ReadPixels_RLE(f,image,image_width,image_height);
  fclose(f);

  glBindTexture(GL_TEXTURE_2D, tex_indicies[i]);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, image_width, image_height, 0, GL_RGB,
      GL_FLOAT, image );
  // Filtering not supported for HDR??
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

  Texture::ShPtr tex (new Texture(tex_names_.at(i)));
  tex->set_index(tex_indicies[i]);
  textures_.insert(TextureTable::value_type(tex_names_.at(i), tex));*/
}

/*
 * Checking the values of the SDL_Surface format appears to be the best way to get the image format.
 */
const GLenum TextureManager::get_surface_format(const SDL_Surface* const surface) {
  GLenum ret;

  if (surface->format->Rmask == 0xFF && surface->format->Gmask == 0xFF00 && surface->format->Bmask == 0xFF0000) {
    if (surface->format->Amask == 0xFF000000) {
      ret = GL_RGBA;
    } else {
      ret = GL_RGB;
    }
  } else {
    std::cout << "Error: unknown image format" << std::endl;
  }

  return ret;
}

/*
 * Searches through the list of loaded textures for one with a name of path. Returns it if it was found, otherwise tries
 * to load an image from that path. If loading fails, returns the default texture.
 */
Texture::ShPtr TextureManager::get_texture(const std::string& path) {
  Texture::ShPtr ret;
  TextureTable::iterator iter = textures_.find(path);

  if (iter != textures_.end()) {
    ret = iter->second;
  } else {
    if (load_texture(path)) {
      ret = textures_.find(path)->second;
    } else {
      std::cout << "Error: texture <" << path << "> not found" << std::endl;
      ret = textures_.find("__err_texture")->second;
    }
  }

  return ret;
}
