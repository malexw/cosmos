#ifndef COSMOS_GAMEOBJECT_H_
#define COSMOS_GAMEOBJECT_H_

#include <boost/shared_ptr.hpp>
//#include <vector>
#include <string>

//#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"

#include "InputHandler.hpp"
#include "Renderable.hpp"
#include "Transform.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
//#include "Vector3f.hpp"

/*
 * A class for representing the dynamic objects in the game
 */
class GameObject {
 public:
	typedef boost::shared_ptr<GameObject> ShPtr;

	GameObject(): id_(nextId_++) {}
  const unsigned int id() const { return id_; }

  GameObject& set_transform(Transform::ShPtr t) { transform_ = t; return *this; }
  GameObject& set_renderable(Renderable::ShPtr r) { renderable_ = r; return *this; }

 protected:
  Transform::ShPtr transform_;
  Renderable::ShPtr renderable_;
  //CollidableObject collidable_;
  InputHandler::ShPtr input_;

 private:
  const unsigned int id_;
  static unsigned int nextId_;
  
  DISALLOW_COPY_AND_ASSIGN(GameObject);
};

#endif
