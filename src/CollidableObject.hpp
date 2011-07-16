#ifndef COSMOS_COLLIDABLEOBJECT_H_
#define COSMOS_COLLIDABLEOBJECT_H_

#include <boost/shared_ptr.hpp>
#include <iostream>
//#include <vector>
#include <string>

//#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Transform.hpp"
#include "Material.hpp"
#include "Matrix4f.hpp"
#include "Mesh.hpp"
#include "Quaternion.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the collision region of an object
 */
class CollidableObject {
 public:
	typedef boost::shared_ptr<CollidableObject> ShPtr;

  //CollidableObject(GameObject::ShPtr parent): parent_(parent) id_(parent.id()), scale_(Vector3f(1.0f, 1.0f, 1.0f)) {}
	//CollidableObject(unsigned int id): id_(id), scale_(Vector3f(1.0f, 1.0f, 1.0f)) {}
  CollidableObject(unsigned int id);
  const unsigned int id() const { return id_; }
  
  void update(float delta);
  void check(CollidableObject::ShPtr rhs);
  //void load();
  void render_collision();
  CollidableObject& set_velocity(const Vector3f& velo) { velo_ = velo; return *this; }
  
  // S Q T
  CollidableObject& set_scale(const Vector3f& scale) { scale_ = scale; return *this; }
  //CollidableObject& set_quat(const Quaternion& quat) { quat_ = quat; return *this; }
  //CollidableObject& set_translate(const Vector3f& trans) { translate_ = trans; return *this; }
  
  //Vector3f get_position() { return translate_; }
  Vector3f get_scale() { return scale_; }
  
  Transform::ShPtr get_transform() { return transform_; }
  
  //CollidableObject& rotate(const Quaternion& q);
  //CollidableObject& rotateX(float angle);
  //CollidableObject& rotateY(float angle);
  //CollidableObject& rotateZ(float angle);
  //CollidableObject& scale(const Vector3f& scale);
  //CollidableObject& translate(const Vector3f& translation);
  //void print() const { std::cout << trans_ << std::endl; };

 private:
  Transform::ShPtr transform_;
  const unsigned int id_;
  //Matrix4f trans_;
  //Matrix4f itrans_;
  Vector3f scale_;
  //Quaternion quat_;
  //Vector3f translate_;
  Vector3f velo_;
  
  DISALLOW_COPY_AND_ASSIGN(CollidableObject);
};

#endif
