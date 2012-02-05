#ifndef COSMOS_GAMEOBJECTMANAGER_H_
#define COSMOS_GAMEOBJECTMANAGER_H_

#include <list>

#include <boost/shared_ptr.hpp>

// Components
#include "Transform.hpp"
#include "Renderable.hpp"
#include "CollidableObject.hpp"
#include "InputHandler.hpp"
// end of Components

#include "message.hpp"
#include "util.hpp"


/*
 * A class for managing things in the game
 */
class GameObjectManager {
 public:
  typedef boost::shared_ptr<GameObjectManager> ShPtr;

  static const char COMPONENT_TRANSFORM;
  static const char COMPONENT_RENDERABLE;
  static const char COMPONENT_COLLIDABLE;
  static const char COMPONENT_INPUTHANDLER;

  GameObjectManager() {}

  unsigned int spawn(char components);

  void message_transform(unsigned int id, Message::ShPtr msg);
  void message_renderable(unsigned int id, Message::ShPtr msg);
  void message_collidable(unsigned int id, Message::ShPtr msg);
  void message_input_handler(unsigned int id, Message::ShPtr msg);
  
  Transform::ShPtr get_transform(unsigned int id);
  Renderable::ShPtr get_renderable(unsigned int id);
  CollidableObject::ShPtr get_collidable(unsigned int id);
  InputHandler::ShPtr get_input_handler(unsigned int id);
  
  void draw_geometries();
  
  void update_collidables(float delta);
  void check_collisions();

 private:
  static unsigned int s_next_id;

  std::list<Transform::ShPtr> transforms_;
  std::list<Renderable::ShPtr> renderables_;
  std::list<CollidableObject::ShPtr> collidables_;
  std::list<InputHandler::ShPtr> input_handlers_;

  template<typename T>
  T find(std::list<T> list, unsigned int id);

  DISALLOW_COPY_AND_ASSIGN(GameObjectManager);
};

#endif
