#ifndef COSMOS_GAMEOBJECTMANAGER_H_
#define COSMOS_GAMEOBJECTMANAGER_H_

#include <boost/shared_ptr.hpp>
//#include <vector>
#include <list>
#include <string>

#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"

#include "GameObject.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
//#include "Vector3f.hpp"

/*
 * A class for managing things in the game
 */
class GameObjectManager {
 public:
	typedef boost::shared_ptr<GameObjectManager> ShPtr;

	GameObjectManager() {}
  static GameObjectManager& get();
  
  void add_object(GameObject::ShPtr ob);
  GameObject::ShPtr get_object(unsigned int id);

 private:
  std::list<GameObject::ShPtr> objects_;
  
  DISALLOW_COPY_AND_ASSIGN(GameObjectManager);
};

#endif
