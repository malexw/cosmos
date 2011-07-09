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
#include "util.hpp"
//#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the space transform of an object
 */
class Transform {
 public:
	typedef boost::shared_ptr<Transform> ShPtr;

	Transform(unsigned int id): id_(id) {}
  const unsigned int id() const { return id_; }
  
  void apply();
  void load();
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
  
  DISALLOW_COPY_AND_ASSIGN(Transform);
};

#endif
