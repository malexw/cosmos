#include <iostream>

#include "FontManager.hpp"
#include "MeshManager.hpp"
#include "ResourceManager.hpp"
#include "TextureManager.hpp"

ResourceManager::ResourceManager() {
	init();
}

void ResourceManager::init() {
  TextureManager::get();
  FontManager::get();
  MeshManager::get();
}

/*
 * Singleton pattern
 */
ResourceManager& ResourceManager::get() {
  static ResourceManager instance;
  return instance;
}
