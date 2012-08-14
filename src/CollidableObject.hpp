#ifndef COSMOS_COLLIDABLEOBJECT_H_
#define COSMOS_COLLIDABLEOBJECT_H_

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <iostream>
#include <string>

#include "Renderer.hpp"

#include "Transform.hpp"
#include "Material.hpp"
#include "Matrix4f.hpp"
#include "Mesh.hpp"
#include "Message.hpp"
#include "Quaternion.hpp"
#include "Ray.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing the collision region of an object
 */
class CollidableObject {
 public:
  typedef boost::shared_ptr<CollidableObject> ShPtr;

  static const unsigned int TYPE_SPHERE;
  static const unsigned int TYPE_CAPSULE;

  CollidableObject(unsigned int id, unsigned int type, Transform::ShPtr transform);
  const unsigned int id() const { return id_; }
  const unsigned int get_type() const { return type_; }

  void handle_message(Message::ShPtr msg);

  void update(float delta);
  void check(CollidableObject::ShPtr rhs);
  //void gjk(CollidableObject::ShPtr rhs);
  void render_collision();
  //Vector3f supportSS(CollidableObject::ShPtr rhs, const Vector3f direction);
  bool query(const Ray& ray);

  Vector3f get_velocity() { return velo_; }
  CollidableObject& set_velocity(const Vector3f& velo) { velo_ = velo; return *this; }
  CollidableObject& set_scale(const Vector3f& scale) { scale_ = scale; return *this; }

  CollidableObject& set_callback(boost::function<void (float delta)> callback) { update_callback_ = callback; }

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

  boost::function<void (float delta)> update_callback_;

  DISALLOW_COPY_AND_ASSIGN(CollidableObject);
};

#endif
