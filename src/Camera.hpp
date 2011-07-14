#ifndef COSMOS_CAMERA_H_
#define COSMOS_CAMERA_H_

#include <boost/shared_ptr.hpp>
#include <iostream>
//#include <vector>
//#include <string>

//#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

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

	Camera(): scale_(Vector3f(1.0f, 1.0f, 1.0f)) {}
  
  void apply();
  
  Camera& set_direction(const Vector3f& dir);
  Camera& rotate(const Vector3f& axis, float angle);
  Camera& rotate_relative(const Vector3f& axis, float angle);
  Camera& shift(const Vector3f& dir);
  Camera& set_velocity(const Vector3f& velo) { velo_ = velo; return *this; }
  void update(float delta);
  
  static Matrix4f matrixFromPositionDirection(Vector3f position, Vector3f direction);
  
  // S Q T
  Camera& set_scale(const Vector3f& scale) { scale_ = scale; return *this; }
  Camera& set_quat(const Quaternion& quat) { quat_ = quat; return *this; }
  Camera& set_translate(const Vector3f& trans) { translate_ = trans; return *this; }

 private:
  Vector3f scale_;
  Quaternion quat_;
  Vector3f translate_;
  Vector3f velo_;
  
  DISALLOW_COPY_AND_ASSIGN(Camera);
};

#endif
