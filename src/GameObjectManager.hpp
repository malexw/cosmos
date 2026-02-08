#ifndef COSMOS_GAMEOBJECTMANAGER_H_
#define COSMOS_GAMEOBJECTMANAGER_H_

#include <list>
#include <string>
#include <unordered_map>

#include <memory>

#include "GameObject.hpp"
#include "Vector3f.hpp"
#include "util.hpp"

/*
 * A class for managing things in the game
 */
class GameObjectManager {
 public:
	typedef std::shared_ptr<GameObjectManager> ShPtr;

	GameObjectManager() {}
  static GameObjectManager& get();
  
  void add_object(GameObject::ShPtr ob);
  GameObject::ShPtr get_object(unsigned int id);
  const std::list<GameObject::ShPtr>& objects() const { return objects_; }

  GameObject::ShPtr create_object(const std::string& mesh_tres, const Vector3f& position);
  GameObject::ShPtr create_object(const std::string& mesh_tres, const Vector3f& position, const std::string& name);
  GameObject::ShPtr get_object_by_name(const std::string& name) const;

 private:
  // TODO make this a hashmap
  std::list<GameObject::ShPtr> objects_;
  std::unordered_map<std::string, GameObject::ShPtr> named_objects_;
  
  DISALLOW_COPY_AND_ASSIGN(GameObjectManager);
};

#endif
