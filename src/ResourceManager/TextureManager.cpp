#include <iostream>

#include "SDL/SDL_image.h"

#include "TextureManager.hpp"

TextureManager::TextureManager() 
  : loaded_(false) {
	init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' textures list.
 * In the future, TextureMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void TextureManager::init() {
	tex_names_.push_back(std::string("res/textures/default.png"));
	tex_names_.push_back(std::string("res/textures/terminal.png"));
  tex_names_.push_back(std::string("res/textures/tronish.png"));
  tex_names_.push_back(std::string("res/textures/normal_map.png"));
  tex_names_.push_back(std::string("res/textures/decal-test.png"));
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
	
	unsigned int tex_count = tex_names_.size() + 1;
	unsigned int tex_indicies[tex_count];
	SDL_Surface *textureImage[tex_count];
	glGenTextures(tex_count, tex_indicies);

	// TODO: OMG hacks - FIXME soon!
	for (int i = 0; i < tex_count-2; ++i) {
		//textureImage[i] = SDL_LoadBMP(tex_names_.at(i).c_str());
		textureImage[i] = IMG_Load(tex_names_.at(i).c_str());
		if (textureImage[i]) {		
			glBindTexture(GL_TEXTURE_2D, tex_indicies[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, 3, textureImage[i]->w, textureImage[i]->h, 0, GL_RGB,
					GL_UNSIGNED_BYTE, textureImage[i]->pixels );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			
			Texture::ShPtr tex (new Texture(tex_names_.at(i)));
			tex->set_index(tex_indicies[i]);
			textures_.push_back(tex);

			SDL_FreeSurface(textureImage[i]);
		} else {
			std::cout << "TextureMan: Error loading texture " << tex_names_.at(i) << std::endl;
		}
	}
  for (int i = tex_count-2; i < tex_count-1; ++i) {
		//textureImage[i] = SDL_LoadBMP(tex_names_.at(i).c_str());
		textureImage[i] = IMG_Load(tex_names_.at(i).c_str());
		if (textureImage[i]) {		
			glBindTexture(GL_TEXTURE_2D, tex_indicies[i]);
			glTexImage2D(GL_TEXTURE_2D, 0, 4, textureImage[i]->w, textureImage[i]->h, 0, GL_RGBA,
					GL_UNSIGNED_BYTE, textureImage[i]->pixels );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
			glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			
			Texture::ShPtr tex (new Texture(tex_names_.at(i)));
			tex->set_index(tex_indicies[i]);
			textures_.push_back(tex);

			SDL_FreeSurface(textureImage[i]);
		} else {
			std::cout << "TextureMan: Error loading texture " << tex_names_.at(i) << std::endl;
		}
	}
  glBindTexture(GL_TEXTURE_2D, tex_indicies[tex_count-1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);    
  // Remove artefact on the edges of the shadowmap
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
  //glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor );
  // No need to force GL_DEPTH_COMPONENT24, drivers usually give you the max precision if available 
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 960, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
  Texture::ShPtr shadow (new Texture("shadow_map"));
  shadow->set_index(tex_indicies[tex_count-1]);
  textures_.push_back(shadow);
  
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
	foreach (Texture::ShPtr tex, textures_) {
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
