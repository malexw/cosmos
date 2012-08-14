#include <iostream>

#include "GameObjectManager.hpp"

const char GameObjectManager::COMPONENT_TRANSFORM = 0x01;
const char GameObjectManager::COMPONENT_RENDERABLE = 0x02;
const char GameObjectManager::COMPONENT_COLLIDABLE = 0x04;
const char GameObjectManager::COMPONENT_INPUTHANDLER = 0x08;

unsigned int GameObjectManager::s_next_id = 0;

unsigned int GameObjectManager::spawn(char components) {
  unsigned int id = s_next_id;
  s_next_id += 1;

  // Local reference so we can use it in the constructor for Renderables and Collidables
  Transform::ShPtr transform;

  if ((components & COMPONENT_TRANSFORM) ||
      (components & COMPONENT_RENDERABLE) ||
      (components & COMPONENT_COLLIDABLE)) {
    transform.reset(new Transform(id));
    transforms_.push_back(transform);
  }

  if (components & COMPONENT_RENDERABLE) {
    Renderable::ShPtr r(new Renderable(id, transform, mat_man_, mesh_man_));
    renderables_.push_back(r);
  }

  if (components & COMPONENT_COLLIDABLE) {
    CollidableObject::ShPtr c(new CollidableObject(id, CollidableObject::TYPE_SPHERE, transform));
    collidables_.push_back(c);
  }

  if (components & COMPONENT_INPUTHANDLER) {
    // TODO InputHandler shouldn't be abstract, will accept a callback
    //InputHandler::ShPtr i(new InputHandler(id));
    //input_handlers_.push_back(i);
  }

  return id;
}

void GameObjectManager::message_transform(unsigned int id, Message::ShPtr msg) {
  (find_component<Transform::ShPtr>(transforms_, id))->handle_message(msg);
}

void GameObjectManager::message_renderable(unsigned int id, Message::ShPtr msg) {
  (find_component<Renderable::ShPtr>(renderables_, id))->handle_message(msg);
}

void GameObjectManager::message_collidable(unsigned int id, Message::ShPtr msg) {
  (find_component<CollidableObject::ShPtr>(collidables_, id))->handle_message(msg);
}

void GameObjectManager::message_input_handler(unsigned int id, Message::ShPtr msg) {
  // TODO Implement this
  //(find_component<InputHandler::ShPtr>(input_handlers_, id))->handle_message(msg);
}

Transform::ShPtr GameObjectManager::get_transform(unsigned int id) {
  return find_component<Transform::ShPtr>(transforms_, id);
}

Renderable::ShPtr GameObjectManager::get_renderable(unsigned int id) {
  return find_component<Renderable::ShPtr>(renderables_, id);
}

CollidableObject::ShPtr GameObjectManager::get_collidable(unsigned int id) {
  return find_component<CollidableObject::ShPtr>(collidables_, id);
}

InputHandler::ShPtr GameObjectManager::get_input_handler(unsigned int id) {
  return find_component<InputHandler::ShPtr>(input_handlers_, id);
}

template<typename T>
T GameObjectManager::find_component(std::list<T> list, unsigned int id) {
  foreach (T item, list) {
    if (item->id() == id) {
      return item;
    }
  }
}

void GameObjectManager::draw_geometries() {

}

void GameObjectManager::update_collidables(float delta) {
  foreach (CollidableObject::ShPtr c, collidables_) {
    c->update(delta);
  }
}

void GameObjectManager::check_collisions() {
  // Check against every other object for a collision. Horribly inefficient.
  for (std::list<CollidableObject::ShPtr>::const_iterator ci = collidables_.begin(); ci != collidables_.end(); ++ci) {
    for (std::list<CollidableObject::ShPtr>::const_iterator rhs = collidables_.begin(); rhs != collidables_.end(); ++rhs) {
      //rhs++;
      //std::cout << "Checking <" << (*ci)->id() << ">, <" << (*rhs)->id() << ">" << std::endl;
      (*ci)->check(*rhs);
    }
  }
}
