#ifndef COSMOS_CAMERA_H_
#define COSMOS_CAMERA_H_

#include <memory>
#include <iostream>

#include "CollidableObject.hpp"
#include "Matrix4f.hpp"
#include "Quaternion.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the space Camera of an object
 */
class Camera {
 public:
	typedef std::shared_ptr<Camera> ShPtr;

	Camera(): scale_(Vector3f::ONES), collision_radius_(3.0f) {}
  
  Camera& set_collidable(CollidableObject::ShPtr c) { collidable_ = c; return *this; }
  CollidableObject::ShPtr get_collidable() { return collidable_; }

  static Matrix4f matrixFromPositionDirection(Vector3f position, Vector3f direction);

 private:
  CollidableObject::ShPtr collidable_;
  float collision_radius_;
  Vector3f scale_;
  
  DISALLOW_COPY_AND_ASSIGN(Camera);
};

#endif
