#ifndef COSMOS_GAMEOBJECTMANAGER_H_
#define COSMOS_GAMEOBJECTMANAGER_H_

#include <list>

#include <memory>

#include "GameObject.hpp"
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

 private:
  // TODO make this a hashmap
  std::list<GameObject::ShPtr> objects_;
  
  DISALLOW_COPY_AND_ASSIGN(GameObjectManager);
};

#endif
