#include <cmath>
#include <vector>

#include "GameObjectManager.hpp"
#include "CollidableObject.hpp"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const unsigned int CollidableObject::TYPE_SPHERE = 0;
const unsigned int CollidableObject::TYPE_CAPSULE = 1;

GLuint CollidableObject::sphere_vbo_ = 0;
GLuint CollidableObject::cylinder_vbo_ = 0;
int CollidableObject::sphere_vertex_count_ = 0;
int CollidableObject::cylinder_vertex_count_ = 0;
bool CollidableObject::geometry_initialized_ = false;

static void generateSphere(std::vector<float>& verts, int slices, int stacks) {
  for (int i = 0; i < stacks; ++i) {
    float phi0 = (float)M_PI * (float)i / (float)stacks;
    float phi1 = (float)M_PI * (float)(i + 1) / (float)stacks;
    for (int j = 0; j < slices; ++j) {
      float theta0 = 2.0f * (float)M_PI * (float)j / (float)slices;
      float theta1 = 2.0f * (float)M_PI * (float)(j + 1) / (float)slices;

      float x00 = sinf(phi0)*cosf(theta0), y00 = sinf(phi0)*sinf(theta0), z00 = cosf(phi0);
      float x10 = sinf(phi1)*cosf(theta0), y10 = sinf(phi1)*sinf(theta0), z10 = cosf(phi1);
      float x01 = sinf(phi0)*cosf(theta1), y01 = sinf(phi0)*sinf(theta1), z01 = cosf(phi0);
      float x11 = sinf(phi1)*cosf(theta1), y11 = sinf(phi1)*sinf(theta1), z11 = cosf(phi1);

      verts.push_back(x00); verts.push_back(y00); verts.push_back(z00);
      verts.push_back(x10); verts.push_back(y10); verts.push_back(z10);
      verts.push_back(x01); verts.push_back(y01); verts.push_back(z01);

      verts.push_back(x10); verts.push_back(y10); verts.push_back(z10);
      verts.push_back(x11); verts.push_back(y11); verts.push_back(z11);
      verts.push_back(x01); verts.push_back(y01); verts.push_back(z01);
    }
  }
}

static void generateCylinder(std::vector<float>& verts, int slices, int stacks) {
  for (int i = 0; i < stacks; ++i) {
    float z0 = (float)i / (float)stacks;
    float z1 = (float)(i + 1) / (float)stacks;
    for (int j = 0; j < slices; ++j) {
      float theta0 = 2.0f * (float)M_PI * (float)j / (float)slices;
      float theta1 = 2.0f * (float)M_PI * (float)(j + 1) / (float)slices;

      float x0 = cosf(theta0), y0 = sinf(theta0);
      float x1 = cosf(theta1), y1 = sinf(theta1);

      verts.push_back(x0); verts.push_back(y0); verts.push_back(z0);
      verts.push_back(x0); verts.push_back(y0); verts.push_back(z1);
      verts.push_back(x1); verts.push_back(y1); verts.push_back(z0);

      verts.push_back(x0); verts.push_back(y0); verts.push_back(z1);
      verts.push_back(x1); verts.push_back(y1); verts.push_back(z1);
      verts.push_back(x1); verts.push_back(y1); verts.push_back(z0);
    }
  }
}

void CollidableObject::initGeometry() {
  if (geometry_initialized_) return;

  std::vector<float> sphereVerts;
  generateSphere(sphereVerts, 16, 16);
  sphere_vertex_count_ = sphereVerts.size() / 3;
  glGenBuffers(1, &sphere_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sphereVerts.size() * sizeof(float), sphereVerts.data(), GL_STATIC_DRAW);

  std::vector<float> cylinderVerts;
  generateCylinder(cylinderVerts, 16, 16);
  cylinder_vertex_count_ = cylinderVerts.size() / 3;
  glGenBuffers(1, &cylinder_vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, cylinder_vbo_);
  glBufferData(GL_ARRAY_BUFFER, cylinderVerts.size() * sizeof(float), cylinderVerts.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  geometry_initialized_ = true;
}

CollidableObject::CollidableObject(unsigned int id, unsigned int type)
 : id_(id), type_(type), scale_(Vector3f(1.0f, 1.0f, 1.0f)) {
  transform_ = GameObjectManager::get().get_object(id)->get_transform();
  initGeometry();
}

void CollidableObject::update(float delta) {
  transform_->set_translate(transform_->get_position() + (delta * (transform_->get_rotation() * velo_)));
}

void CollidableObject::check(CollidableObject::ShPtr rhs) {
  Vector3f distance;
  float length, actual_distance;
  Vector3f rhs_size = rhs->get_scale();
  switch (rhs->get_type()) {
    case CollidableObject::TYPE_CAPSULE: {
      // Check against the first sphere
      Vector3f sphere1_pos = rhs->get_transform()->get_position() + Vector3f(0, 0, scale_.z()/2);//(rhs->get_transform()->get_rotation() * Vector3f(0, 0, scale_.z()/2));
      distance = (transform_->get_position()+Vector3f(0, 0, scale_.z()/2)) - sphere1_pos;
      length = distance.lengthSquare();
      //std::cout << "Distance: " << length << std::endl;
      actual_distance = (scale_.x() + rhs_size.x()) * (scale_.x() + rhs_size.x());

      if (actual_distance > length) {
        float correction = sqrt(actual_distance) - sqrt(length);
        distance = correction * distance.normalize();
        //std::cout << "correction: " << distance << std::endl;
        transform_->set_translate(distance + transform_->get_position());
      }
      
      Vector3f sphere2_pos = rhs->get_transform()->get_position() - Vector3f(0, 0, scale_.z()/2);//(rhs->get_transform()->get_rotation() * Vector3f(0, 0, -(scale_.z()/2)));
      distance = transform_->get_position() - sphere2_pos;
      length = distance.lengthSquare();
      //std::cout << "Distance: " << length << std::endl;
      actual_distance = (scale_.x() + rhs_size.x()) * (scale_.x() + rhs_size.x());

      if (actual_distance > length) {
        float correction = sqrt(actual_distance) - sqrt(length);
        distance = correction * distance.normalize();
        //std::cout << "correction: " << distance << std::endl;
        transform_->set_translate(distance + transform_->get_position());
      }
      
      // Check against the second sphere
      break; }
    case CollidableObject::TYPE_SPHERE:
      distance = transform_->get_position() - rhs->get_transform()->get_position();
      length = distance.lengthSquare();
      actual_distance = (scale_.x() + rhs_size.x()) * (scale_.x() + rhs_size.x());

      if (actual_distance > length) {
        float correction = sqrt(actual_distance) - sqrt(length);
        distance = correction * distance.normalize();
        transform_->set_translate(distance + transform_->get_position());
      }
      break;
  }
}

/*void CollidableObject::gjk(CollidableObject::ShPtr rhs) {
  Vector3f search_direction = (transform_->get_position() - rhs->get_transform()->get_position()).normalize();
  simplex_[0] = supportSS(rhs, search_direction);

  search_direction = -search_direction;
  simplex_[1] = supportSS(rhs, search_direction);
  
  if (simplex_[1].dot(search_direction) <= 0) {
    //std::cout << "Fail 1" << std::endl;
    return;
  }
  
  //std::cout << "Pass 1" << std::endl;
  Vector3f edge1 = simplex_[1] - simplex_[0];
  search_direction = edge1.cross(-simplex_[1]).cross(edge1);
  simplex_[2] = supportSS(rhs, search_direction);
  
  if (simplex_[2].dot(search_direction) <= 0) {
    //std::cout << "Fail 2" << std::endl;
    return;
  }
  
  Vector3f edge2 = simplex_[2] - simplex_[1];
  search_direction = edge2.cross(-simplex_[2]).cross(edge2);
  
  if (search_direction.dot(simplex_[2]) <= 0) {
    //std::cout << "Fail 3" << std::endl;
    return;
  }
  Vector3f edge3 = simplex_[2] - simplex_[0];
  if (edge3.cross(edge2).cross(edge3).dot > 0) {
    return;
  }
  
  simplex_[3] = supportSS(rhs, search_direction);
  
  if (simplex_[3].dot(search_direction) > 0) {
    return;
  }
  Vector3f edge4 = simplex_[3] - simplex_[2];
  search_direction = edge4.cross(-simplex_[3]).cross(edge4);
  
  if (search_direction.dot(simplex_[3]) > 0) {
    return;
  }
  
  std::cout << "Hit" << std::endl;
}*/

void CollidableObject::render_collision() {
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHTING);

  // Disable unused client states — VBOs only contain position data
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);

  switch (type_) {
    case CollidableObject::TYPE_CAPSULE: {
      glPushMatrix();
      glTranslatef(0.0f, 0.0f, -(scale_.z()/2));
      // Cylinder (unit geometry scaled to radius=scale_.x(), height=scale_.z())
      glPushMatrix();
      glScalef(scale_.x(), scale_.x(), scale_.z());
      glBindBuffer(GL_ARRAY_BUFFER, cylinder_vbo_);
      glVertexPointer(3, GL_FLOAT, 0, (void*)0);
      glDrawArrays(GL_TRIANGLES, 0, cylinder_vertex_count_);
      glPopMatrix();
      // Sphere at top
      glTranslatef(0.0f, 0.0f, scale_.z());
      glPushMatrix();
      glScalef(scale_.x(), scale_.x(), scale_.x());
      glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_);
      glVertexPointer(3, GL_FLOAT, 0, (void*)0);
      glDrawArrays(GL_TRIANGLES, 0, sphere_vertex_count_);
      glPopMatrix();
      // Sphere at bottom
      glTranslatef(0.0f, 0.0f, -(scale_.z()));
      glPushMatrix();
      glScalef(scale_.x(), scale_.x(), scale_.x());
      glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_);
      glVertexPointer(3, GL_FLOAT, 0, (void*)0);
      glDrawArrays(GL_TRIANGLES, 0, sphere_vertex_count_);
      glPopMatrix();
      glPopMatrix();
      break;
    }
    case CollidableObject::TYPE_SPHERE: {
      glPushMatrix();
      glScalef(scale_.x(), scale_.x(), scale_.x());
      glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_);
      glVertexPointer(3, GL_FLOAT, 0, (void*)0);
      glDrawArrays(GL_TRIANGLES, 0, sphere_vertex_count_);
      glPopMatrix();
      break;
    }
  }

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Re-enable client states
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  glEnable(GL_LIGHTING);
  glEnable(GL_CULL_FACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

/*Vector3f CollidableObject::supportSS(CollidableObject::ShPtr rhs, const Vector3f direction) {
  Vector3f lhspoint = transform_->get_position() + (scale_.x() * direction);
  Vector3f rhspoint = rhs->get_transform()->get_position() + (rhs->get_scale().x() * (-direction));
  //std::cout << lhspoint << " " << rhspoint << std::endl;
  return lhspoint - rhspoint;
}*/
