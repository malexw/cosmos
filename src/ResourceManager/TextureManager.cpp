#include <iostream>
#include <memory>
#include <vector>
#include <cmath>

#include <SDL3/SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "rgbe.h"

#include "TextureManager.hpp"

TextureManager::TextureManager() 
  : loaded_(false) {
	init();
}

void TextureManager::init() {
  load_textures();
}

/*
 * Singleton pattern
 */
TextureManager& TextureManager::get() {
  static TextureManager instance;
  return instance;
}

Texture::ShPtr TextureManager::load_texture(const std::string& path,
                                            TexFilter filter, TexWrap wrap) {
  // Return existing texture if already loaded
  for (const Texture::ShPtr& tex : textures_) {
    if (tex->is_name(path)) {
      return tex;
    }
  }

  GLenum gl_filter = (filter == TexFilter::Nearest) ? GL_NEAREST : GL_LINEAR;
  GLenum gl_wrap;
  switch (wrap) {
    case TexWrap::Clamp:  gl_wrap = GL_CLAMP_TO_EDGE; break;
    case TexWrap::Mirror: gl_wrap = GL_MIRRORED_REPEAT; break;
    default:              gl_wrap = GL_REPEAT; break;
  }

  GLuint tex_index;
  glGenTextures(1, &tex_index);

  if (stbi_is_hdr(path.c_str())) {
    // HDR float image via RGBE
    int image_width, image_height;
    FILE* f = fopen(path.c_str(), "rb");
    if (!f) {
      std::cout << "TextureMan: Error loading texture " << path << std::endl;
      return Texture::ShPtr();
    }

    RGBE_ReadHeader(f, &image_width, &image_height, nullptr);
    float* image = (float *)malloc(sizeof(float) * 3 * image_width * image_height);
    RGBE_ReadPixels_RLE(f, image, image_width, image_height);
    fclose(f);

    glBindTexture(GL_TEXTURE_2D, tex_index);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, image_width, image_height, 0, GL_RGB,
        GL_FLOAT, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap);

    // Compute log-average luminance for a reasonable default exposure
    int pixel_count = image_width * image_height;
    double log_sum = 0.0;
    for (int p = 0; p < pixel_count; ++p) {
      float r = image[p * 3];
      float g = image[p * 3 + 1];
      float b = image[p * 3 + 2];
      float lum = 0.30f * r + 0.59f * g + 0.11f * b;
      log_sum += std::log(0.001f + lum);
    }
    float avg_lum = std::exp(log_sum / pixel_count);
    float default_exposure = 0.18f / avg_lum;
    std::cout << "HDR avg luminance: " << avg_lum << ", default exposure: " << default_exposure << std::endl;

    auto tex = std::make_shared<Texture>(path);
    tex->set_index(tex_index);
    tex->set_default_exposure(default_exposure);
    textures_.push_back(tex);

    free(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
  } else {
    // LDR image — auto-detect channel count
    int width, height, channels;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!data) {
      std::cout << "TextureMan: Error loading texture " << path << std::endl;
      return Texture::ShPtr();
    }

    GLenum internal_fmt = (channels == 4) ? GL_RGBA8 : GL_RGB8;
    GLenum fmt = (channels == 4) ? GL_RGBA : GL_RGB;

    glBindTexture(GL_TEXTURE_2D, tex_index);
    glTexImage2D(GL_TEXTURE_2D, 0, internal_fmt, width, height, 0, fmt,
        GL_UNSIGNED_BYTE, data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap);

    auto tex = std::make_shared<Texture>(path);
    tex->set_index(tex_index);
    textures_.push_back(tex);

    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return tex;
  }
}

/*
 * load_textures iterates through each texture in the tex_names_ collection and attempts to load the texture.
 */
void TextureManager::load_textures() {
	if (loaded_) {
		std::cout << "TextureMan: Error - textures already loaded" << std::endl;
		return;
	}

  for (const auto& name : tex_names_) {
    load_texture(name);
  }


	glBindTexture(GL_TEXTURE_2D, 0);
	loaded_ = true;
}

/*
 * Uses a dumb linear search to find a texture with the same name. Optimizations welcome!
 */
const Texture::ShPtr TextureManager::get_texture(std::string name) const {
	for (const Texture::ShPtr& tex : textures_) {
		if (tex->is_name(name)) {
			return tex;
		}
	}

  std::cout << "Error: texture <" << name << "> not found" << std::endl;	
	return Texture::ShPtr();
}

//////////////////////////////////////////////////////////////////////////////////////////
//	Normalisation Cube Map.cpp
//	Generate normalisation cube map
//	Downloaded from: www.paulsprojects.net
//	Created:	20th July 2002
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)
//////////////////////////////////////////////////////////////////////////////////////////	
/*void generate_norm_map()
{
	unsigned char* data = new unsigned char[32*32*3];

	int size = 32;
  float halfSize = size/2;
	float offset = 0.5f;
	unsigned char* bytePtr;

  for (int k = 0; k < 6; ++k) {
    
    bytePtr = data;
    
    for (int j = 0; j < size; ++j) {
      for (int i = 0; i < size; ++i) {
        Vector3f v;
        
        switch(k) {
          case 0: v = Vector3f(halfSize, -(j+offset-halfSize), -(i+offset-halfSize)); break;
          case 1: v = Vector3f(-halfSize, -(j+offset-halfSize), i+offset-halfSize); break;
          case 2: v = Vector3f(i+offset-halfSize, halfSize, j+offset-halfSize); break;
          case 3: v = Vector3f(i+offset-halfSize, -halfSize, -(j+offset-halfSize); break;
          case 4: v = Vector3f(i+offset-halfSize, -(j+offset-halfSize), halfSize); break;
          case 5: v = Vector3f(-(i+offset-halfSize), -(j+offset-halfSize), -halfSize); break;
        }
        
        v.to_rgb();

        bytePtr[0] = (unsigned char)(v.x() * 255);
        bytePtr[1] = (unsigned char)(v.y() * 255);
        bytePtr[2] = (unsigned char)(v.z() * 255);

        bytePtr += 3;
      }
    }
    
    switch(k) {
      case 0: glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data); break;
      case 1: glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data); break;
      case 2: glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data); break;
      case 3: glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data); break;
      case 4: glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data); break;
      case 5: glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA8, 32, 32, 0, GL_RGB, GL_UNSIGNED_BYTE, data); break;
    }
  }

	delete [] data;
}*/
