#ifndef COSMOS_COLLIDABLEOBJECT_H_
#define COSMOS_COLLIDABLEOBJECT_H_

#include <memory>
#include <iostream>
#include <string>

#include <glad/gl.h>
#include <glm/glm.hpp>

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
	using ShPtr = std::shared_ptr<CollidableObject>;

  static constexpr unsigned int TYPE_SPHERE = 0;
  static constexpr unsigned int TYPE_CAPSULE = 1;

  CollidableObject(unsigned int id, unsigned int type);
  CollidableObject(Transform::ShPtr transform, unsigned int type);
  unsigned int id() const { return id_; }
  unsigned int get_type() const { return type_; }

  void update(float delta);
  void check(CollidableObject::ShPtr rhs);
  void render_collision(const glm::mat4& projView);

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

  static GLuint sphere_vbo_;
  static GLuint sphere_vao_;
  static GLuint cylinder_vbo_;
  static GLuint cylinder_vao_;
  static int sphere_vertex_count_;
  static int cylinder_vertex_count_;
  static bool geometry_initialized_;
  static void initGeometry();

  CollidableObject(const CollidableObject&) = delete;
  CollidableObject& operator=(const CollidableObject&) = delete;
};

#endif
