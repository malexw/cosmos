#include <iostream>

#include "GameObjectManager.hpp"
#include "ResourceManager/ResourceManager.hpp"
#include "ResourceManager/MeshManager.hpp"

GameObjectManager& GameObjectManager::get() {
  static GameObjectManager instance;
  return instance;
}

void GameObjectManager::add_object(GameObject::ShPtr ob) {
  //TODO if not already added
  objects_.push_back(ob);
}

GameObject::ShPtr GameObjectManager::create_object(const std::string& mesh_tres, const Vector3f& position) {
  ResourceManager::get().load_resource(mesh_tres);

  GameObject::ShPtr obj(new GameObject());
  add_object(obj);

  Transform::ShPtr transform(new Transform(obj->id()));
  transform->set_translate(position);
  obj->set_transform(transform);

  Renderable::ShPtr renderable(new Renderable(obj->id()));
  obj->set_renderable(renderable);

  Mesh::ShPtr mesh = MeshManager::get().get_mesh(mesh_tres);
  renderable->set_mesh(mesh);

  return obj;
}

GameObject::ShPtr GameObjectManager::create_object(const std::string& mesh_tres, const Vector3f& position, const std::string& name) {
  GameObject::ShPtr obj = create_object(mesh_tres, position);
  if (!name.empty()) {
    named_objects_[name] = obj;
  }
  return obj;
}

GameObject::ShPtr GameObjectManager::get_object_by_name(const std::string& name) const {
  auto it = named_objects_.find(name);
  if (it != named_objects_.end()) return it->second;
  std::cout << "ERROR: GameObject not found with name: " << name << std::endl;
  return GameObject::ShPtr();
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
