#ifndef COSMOS_GAMEOBJECT_H_
#define COSMOS_GAMEOBJECT_H_

#include <memory>
#include <string>

#include "CollidableObject.hpp"
#include "Renderable.hpp"
#include "Transform.hpp"
#include "util.hpp"

/*
 * A class for representing the dynamic objects in the game
 */
class GameObject {
 public:
	typedef std::shared_ptr<GameObject> ShPtr;

	GameObject(): id_(nextId_++) {}
  unsigned int id() const { return id_; }

  GameObject& set_transform(Transform::ShPtr t) { transform_ = t; return *this; }
  GameObject& set_renderable(Renderable::ShPtr r) { renderable_ = r; return *this; }
  GameObject& set_collidable(CollidableObject::ShPtr c) { collidable_ = c; return *this; }

  Transform::ShPtr get_transform() { return transform_; }
  Renderable::ShPtr get_renderable() { return renderable_; }
  CollidableObject::ShPtr get_collidable() { return collidable_; }

 protected:
  Transform::ShPtr transform_;
  Renderable::ShPtr renderable_;
  CollidableObject::ShPtr collidable_;

 private:
  const unsigned int id_;
  static unsigned int nextId_;

  DISALLOW_COPY_AND_ASSIGN(GameObject);
};

#endif
