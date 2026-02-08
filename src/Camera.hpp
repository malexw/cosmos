#ifndef COSMOS_CAMERA_H_
#define COSMOS_CAMERA_H_

#include <memory>
#include <iostream>

#include <glm/glm.hpp>

#include "CollidableObject.hpp"
#include "Matrix4f.hpp"
#include "Quaternion.hpp"
#include "Transform.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the space Camera of an object
 */
class Camera {
 public:
	typedef std::shared_ptr<Camera> ShPtr;

	Camera();

  Transform::ShPtr transform() { return transform_; }
  CollidableObject::ShPtr collidable() { return collidable_; }

  static glm::mat4 matrixFromPositionDirection(Vector3f position, Vector3f direction);

 private:
  Transform::ShPtr transform_;
  CollidableObject::ShPtr collidable_;

  DISALLOW_COPY_AND_ASSIGN(Camera);
};

#endif
