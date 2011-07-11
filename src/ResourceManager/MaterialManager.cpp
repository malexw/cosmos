#include <iostream>

#include "SDL/SDL_image.h"

#include "MaterialManager.hpp"
#include "TextureManager.hpp"

MaterialManager::MaterialManager() 
  : loaded_(false) {
	init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' textures list.
 * In the future, TextureMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void MaterialManager::init() {
	mat_names_.push_back(std::string("res/textures/default.png"));
	mat_names_.push_back(std::string("res/textures/terminal.png"));
  mat_names_.push_back(std::string("res/textures/tronish.png"));
  load_materials();
}

/*
 * Singleton pattern
 */
MaterialManager& MaterialManager::get() {
  static MaterialManager instance;
  return instance;
}

/*
 *
 */
void MaterialManager::load_materials() {
	if (loaded_) {
		std::cout << "TextureMan: Error - textures already loaded" << std::endl;
		return;
	}
	
  int mat_count = mat_names_.size();
    
  for (int j = 0; j < mat_count; ++j) {
    // Parse the material file

    Material::ShPtr mat(new Material(mat_names_[j]));
    mat->set_texture(TextureManager::get().get_texture(mat_names_[j]));
    mats_.push_back(mat);
	}
	
	loaded_ = true;	
}

/*
 * Uses a dumb linear search to find a texture with the same name. Optimizations welcome!
 */
const Material::ShPtr MaterialManager::get_material(std::string name) const {
	foreach (Material::ShPtr mat, mats_) {
		if (mat->is_name(name)) {
			return mat;
		}
	}
	
	return Material::ShPtr();
}
