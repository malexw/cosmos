#include <iostream>

#include "GameObjectManager.hpp"

GameObjectManager& GameObjectManager::get() {
  static GameObjectManager instance;
  return instance;
}

void GameObjectManager::add_object(GameObject::ShPtr ob) {
  //TODO if not already added
  objects_.push_back(ob);
}

GameObject::ShPtr GameObjectManager::get_object(unsigned int id) {
  for (const GameObject::ShPtr& object : objects_) {
    if (object->id() == id) {
      return object;
    }
  }
  
  std::cout << "ERROR: GameObject not found" << std::endl;
  return GameObject::ShPtr();
}
