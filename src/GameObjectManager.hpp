#ifndef COSMOS_GAMEOBJECTMANAGER_H_
#define COSMOS_GAMEOBJECTMANAGER_H_

#include <list>

#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"

#include "ResourceManager/MaterialManager.hpp"
#include "ResourceManager/MeshManager.hpp"

// Components
#include "Transform.hpp"
#include "Renderable.hpp"
#include "CollidableObject.hpp"
#include "InputHandler.hpp"
// end of Components

#include "MatrixStack.hpp"
#include "Message.hpp"
#include "Renderer.hpp"
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

  GameObjectManager(MaterialManager::ShPtr mat_man, MeshManager::ShPtr mesh_man, MatrixStack::ShPtr matrix_stack)
   : mat_man_(mat_man), mesh_man_(mesh_man), matrix_stack_(matrix_stack) {}

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
  void render_renderables();

  void handle_input(SDL_Event e);

  void update_collidables(float delta);
  void check_collisions();

 private:
  static unsigned int s_next_id;

  MaterialManager::ShPtr mat_man_;
  MeshManager::ShPtr mesh_man_;

  std::list<Transform::ShPtr> transforms_;
  std::list<Renderable::ShPtr> renderables_;
  std::list<CollidableObject::ShPtr> collidables_;
  std::list<InputHandler::ShPtr> input_handlers_;

  MatrixStack::ShPtr matrix_stack_;

  template<typename T>
  T find_component(std::list<T> list, unsigned int id);

  void upload_model_matrix(const Matrix4f& model_matrix) const;

  DISALLOW_COPY_AND_ASSIGN(GameObjectManager);
};

#endif
