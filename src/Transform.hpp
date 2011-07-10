#ifndef COSMOS_TRANSFORM_H_
#define COSMOS_TRANSFORM_H_

#include <boost/shared_ptr.hpp>
#include <iostream>
//#include <vector>
#include <string>

//#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Material.hpp"
#include "Matrix4f.hpp"
#include "Mesh.hpp"
#include "Quaternion.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the space transform of an object
 */
class Transform {
 public:
	typedef boost::shared_ptr<Transform> ShPtr;

	Transform(unsigned int id): id_(id), scale_(Vector3f(1.0f, 1.0f, 1.0f)) {}
  const unsigned int id() const { return id_; }
  
  void apply();
  void load();
  
  // S Q T
  Transform& set_scale(const Vector3f& scale) { scale_ = scale; return *this; }
  Transform& set_quat(const Quaternion& quat) { quat_ = quat; return *this; }
  Transform& set_translate(const Vector3f& trans) { translate_ = trans; return *this; }
  
  Transform& rotate(const Quaternion& q);
  Transform& rotateX(float angle);
  Transform& rotateY(float angle);
  Transform& rotateZ(float angle);
  Transform& scale(const Vector3f& scale);
  Transform& translate(const Vector3f& translation);
  void print() const { std::cout << trans_ << std::endl; };

 private:
  //static const float M_PI = 3.14159265358979323846;
  const unsigned int id_;
  Matrix4f trans_;
  Matrix4f itrans_;
  Vector3f scale_;
  Quaternion quat_;
  Vector3f translate_; 
  
  DISALLOW_COPY_AND_ASSIGN(Transform);
};

#endif
