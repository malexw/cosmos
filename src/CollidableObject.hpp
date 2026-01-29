#ifndef COSMOS_COLLIDABLEOBJECT_H_
#define COSMOS_COLLIDABLEOBJECT_H_

#include <memory>
#include <iostream>
#include <string>

#include <SDL2/SDL_opengl.h>
#include <GL/glu.h>

#include "Transform.hpp"
#include "Material.hpp"
#include "Matrix4f.hpp"
#include "Mesh.hpp"
#include "Quaternion.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the collision region of an object
 */
class CollidableObject {
 public:
	typedef std::shared_ptr<CollidableObject> ShPtr;

  static const unsigned int TYPE_SPHERE;
  static const unsigned int TYPE_CAPSULE;

  CollidableObject(unsigned int id, unsigned int type);
  unsigned int id() const { return id_; }
  unsigned int get_type() const { return type_; }
  
  void update(float delta);
  void check(CollidableObject::ShPtr rhs);
  //void gjk(CollidableObject::ShPtr rhs);
  void render_collision();
  //Vector3f supportSS(CollidableObject::ShPtr rhs, const Vector3f direction);

  CollidableObject& set_velocity(const Vector3f& velo) { velo_ = velo; return *this; }
  CollidableObject& set_scale(const Vector3f& scale) { scale_ = scale; return *this; }

  Vector3f get_scale() { return scale_; }
  
  Transform::ShPtr get_transform() { return transform_; }

 private:
  Transform::ShPtr transform_;
  const unsigned int id_;
  unsigned int type_;
  Vector3f scale_;
  Vector3f velo_;
  GLUquadricObj* quadric_;
  //Vector3f simplex_[4];
  
  DISALLOW_COPY_AND_ASSIGN(CollidableObject);
};

#endif
