#ifndef COSMOS_CAMERA_H_
#define COSMOS_CAMERA_H_

#include <boost/shared_ptr.hpp>
#include <iostream>

//#include "SDL/SDL_opengl.h"
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"

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

  // These are essentially aliases to make the code look cleaner
  void apply() const { transform_->apply(); } 
  void apply_inverse() const { transform_->apply_inverse(); } 
  void apply_rotation() const { transform_->apply_rotation(); } 
  
  const unsigned int get_id() const { return id_; }
  Transform::ShPtr get_transform() { return transform_; }

  static Matrix4f matrixFromPositionDirection(Vector3f position, Vector3f direction);

 private:
  unsigned int id_;
  
  GameObjectManager::ShPtr gob_man_;
  Transform::ShPtr transform_;
  CollidableObject::ShPtr collidable_;

  DISALLOW_COPY_AND_ASSIGN(Camera);
};

#endif
