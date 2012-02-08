#ifndef COSMOS_CAMERA_H_
#define COSMOS_CAMERA_H_

#include <boost/shared_ptr.hpp>
#include <iostream>

#include "SDL/SDL_opengl.h"

#include "CollidableObject.hpp"
#include "GameObjectManager.hpp"
#include "Matrix4f.hpp"
#include "Quaternion.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the space Camera of an object
 */
class Camera {
 public:
  typedef boost::shared_ptr<Camera> ShPtr;

  Camera(GameObjectManager::ShPtr gob_man);

  //Camera& set_collidable(CollidableObject::ShPtr c) { collidable_ = c; return *this; }
  //CollidableObject::ShPtr get_collidable() { return collidable_; }

  static Matrix4f matrixFromPositionDirection(Vector3f position, Vector3f direction);

 private:
  GameObjectManager::ShPtr gob_man_;
  Transform::ShPtr transform_;
  CollidableObject::ShPtr collidable_;
  float collision_radius_;
  Vector3f scale_;

  DISALLOW_COPY_AND_ASSIGN(Camera);
};

#endif
