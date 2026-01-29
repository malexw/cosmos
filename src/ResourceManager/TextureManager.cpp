#include <iostream>
#include <vector>

#include <SDL2/SDL_opengl.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "rgbe.h"

#include "TextureManager.hpp"

TextureManager::TextureManager() 
  : loaded_(false) {
	init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' textures list.
 * In the future, TextureManager should read from some kind of resource file so we don't need to specify these
 * by hand
 */
void TextureManager::init() {
  // RGB
	tex_names_.push_back(std::string("res/textures/default.png"));
	tex_names_.push_back(std::string("res/textures/terminal.png"));
  tex_names_.push_back(std::string("res/textures/tronish.png"));
  tex_names_.push_back(std::string("res/textures/earth.png"));
  tex_names_.push_back(std::string("res/textures/normal_map.png"));
  // RGBA
  tex_names_.push_back(std::string("res/textures/decal-test.png"));
  tex_names_.push_back(std::string("res/textures/ion.png"));
  tex_names_.push_back(std::string("res/textures/spacebox.png"));
  // HDR
  tex_names_.push_back(std::string("res/textures/grace_cross.hdr"));
  load_textures();
}

/*
 * Singleton pattern
 */
TextureManager& TextureManager::get() {
  static TextureManager instance;
  return instance;
}

/*
 * load_textures iterates through each texture in the tex_names_ collection and attempts to load the texture.
 * This implementation is guided mostly by NeHe Lesson 06:
 * http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=06
 */
void TextureManager::load_textures() {
	if (loaded_) {
		std::cout << "TextureMan: Error - textures already loaded" << std::endl;
		return;
	}
	
	unsigned int tex_count = tex_names_.size() + 2;
	std::vector<unsigned int> tex_indicies(tex_count);
	glGenTextures(tex_count, tex_indicies.data());

	// TODO: OMG hacks - FIXME soon!
	// RGB
  for (unsigned int i = 0; i < tex_count-6; ++i) {
		int width, height, channels;
		unsigned char* data = stbi_load(tex_names_.at(i).c_str(), &width, &height, &channels, 3);
		if (data) {
			glBindTexture(GL_TEXTURE_2D, tex_indicies[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
					GL_UNSIGNED_BYTE, data);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			Texture::ShPtr tex (new Texture(tex_names_.at(i)));
			tex->set_index(tex_indicies[i]);
			textures_.push_back(tex);

			stbi_image_free(data);
		} else {
			std::cout << "TextureMan: Error loading texture " << tex_names_.at(i) << std::endl;
		}
	}
  // RGBA
  for (unsigned int i = tex_count-6; i < tex_count-3; ++i) {
		int width, height, channels;
		unsigned char* data = stbi_load(tex_names_.at(i).c_str(), &width, &height, &channels, 4);
		if (data) {
			glBindTexture(GL_TEXTURE_2D, tex_indicies[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, data);
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

			Texture::ShPtr tex (new Texture(tex_names_.at(i)));
			tex->set_index(tex_indicies[i]);
			textures_.push_back(tex);

			stbi_image_free(data);
		} else {
			std::cout << "TextureMan: Error loading texture " << tex_names_.at(i) << std::endl;
		}
	}
  // HDR
  for (unsigned int i = tex_count-3; i < tex_count-2; ++i) {
		int image_width, image_height;
    FILE* f = fopen(tex_names_[i].c_str(),"rb");
    if (f) {
      RGBE_ReadHeader(f,&image_width,&image_height,NULL);
      float* image = (float *)malloc(sizeof(float)*3*image_width*image_height);
      RGBE_ReadPixels_RLE(f,image,image_width,image_height);
      fclose(f);

      glBindTexture(GL_TEXTURE_2D, tex_indicies[i]);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F_ARB, image_width, image_height, 0, GL_RGB,
          GL_FLOAT, image);
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
      glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

      Texture::ShPtr tex (new Texture(tex_names_.at(i)));
      tex->set_index(tex_indicies[i]);
      textures_.push_back(tex);

      free(image);
    } else {
      std::cout << "TextureMan: Error loading texture " << tex_names_.at(i) << std::endl;
    }
	}
  // Generate a texture as a target for the shadow mapping FBO
  glBindTexture(GL_TEXTURE_2D, tex_indicies[tex_count-2]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  Texture::ShPtr shadow (new Texture("shadow_map"));
  shadow->set_index(tex_indicies[tex_count-2]);
  textures_.push_back(shadow);
  
  // Generate a texture as a target for the HDR rendering FBO
  glBindTexture(GL_TEXTURE_2D, tex_indicies[tex_count-1]);
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, 960, 600, 0, GL_RGBA, GL_FLOAT, 0);
  Texture::ShPtr hdrTarget(new Texture("hdr target"));
  hdrTarget->set_index(tex_indicies[tex_count-1]);
  textures_.push_back(hdrTarget);
  
  /*glBindTexture(GL_TEXTURE_CUBE_MAP, tex_indicies[tex_count]);
  generate_norm_map();
  Texture::ShPtr tex (new Texture("normalization_map"));
  tex->set_index(tex_indicies[tex_count]);
  textures_.push_back(tex);*/
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
