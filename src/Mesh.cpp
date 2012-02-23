#include "Mesh.hpp"

#include <iostream>

void Mesh::add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1,
                        Vector3f v2, Vector2f vt2, Vector3f vn2,
                        Vector3f v3, Vector2f vt3, Vector3f vn3) {

  verticies_.push_back(v1);
  verticies_.push_back(v2);
  verticies_.push_back(v3);

  tex_coords_.push_back(vt1);
  tex_coords_.push_back(vt2);
  tex_coords_.push_back(vt3);

  normals_.push_back(vn1);
  normals_.push_back(vn2);
  normals_.push_back(vn3);

  triangle_count_ += 1;

  size_array_ += 3 * 8;
}

// TODO Don't do this. Subclass mesh into types, eg TriangleMesh, PointMesh, etc.
void Mesh::add_point(Vector3f p) {
  verticies_.push_back(p);
  size_array_ += 3;
}

void Mesh::draw() const {
  if (on_gpu_) {
    glBindVertexArray(vao_address_);

    if (type_ == GL_TRIANGLES) {
      glDrawArrays(GL_TRIANGLES, 0, triangle_count_ * 3);
    } else if (type_ == GL_POINTS) {
      glDrawArrays(GL_POINTS, 0, verticies_.size());
    }

    glBindVertexArray(0);
  } else {
    glClientActiveTexture(GL_TEXTURE0);

    glEnableClientState(GL_VERTEX_ARRAY);
    //glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
    //glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    glVertexPointer(3, GL_FLOAT, 0, &verticies_[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[0]);
    glNormalPointer(GL_FLOAT, 0, &normals_[0]);
    //glColorPointer(3, GL_FLOAT, 0, &colors_[0]);

    //glClientActiveTexture(GL_TEXTURE1);
    //glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[0]);

    glDrawArrays(GL_TRIANGLES, 0, triangle_count_ * 3);

    glDisableClientState(GL_VERTEX_ARRAY);
    //glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY_EXT);
  }
}

void Mesh::deform_relative(unsigned int vertex, Vector3f translation, Vector2f tex_coords, Vector3f normal) {
  if (vertex >= verticies_.size()) {
    return;
  }

  verticies_[vertex] = verticies_[vertex] + translation;
  // TODO fix this when we subclass Mesh
  //tex_coords_[vertex] = tex_coords_[vertex] + tex_coords;
  //normals_[vertex] = normals_[vertex] + normal;
}

void Mesh::deform(unsigned int vertex, Vector3f translation, Vector2f tex_coords, Vector3f normal) {
  if (vertex >= verticies_.size()) {
    return;
  }

  verticies_[vertex] = translation;
  // TODO fix this when we subclass Mesh
  //tex_coords_[vertex] = tex_coords;
  //normals_[vertex] = normal;
}

void Mesh::upload_to_gpu() {
  upload_internal(GL_STATIC_DRAW);
}

void Mesh::upload_to_gpu_type(GLenum usage) {
  upload_internal(usage);
}

void Mesh::upload_internal(GLenum usage) {

  boost::shared_array<float> attrib_array = to_array();

  glGenVertexArrays(1, &vao_address_);
  glBindVertexArray(vao_address_);

  glGenBuffers(1, &vbo_address_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);
  glBufferData(GL_ARRAY_BUFFER, size_array_*sizeof(float), attrib_array.get(), usage);

  if (type_ == GL_TRIANGLES) {
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3/*position x, y, z*/, GL_FLOAT, GL_TRUE, 32/*stride*/, 0/*start*/);
    glVertexAttribPointer(1, 2/*tex u, v*/, GL_FLOAT, GL_TRUE, 32/*stride*/, (void*)12/*start*/);
    glVertexAttribPointer(2, 3/*normal x, y, z*/, GL_FLOAT, GL_TRUE, 32/*stride*/, (void*)20/*start*/);
  } else if (type_ == GL_POINTS) {
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3/*position x, y, z*/, GL_FLOAT, GL_TRUE, 0/*stride*/, 0/*start*/);
  }

  glBindVertexArray(0);
  on_gpu_ = true;
}

void Mesh::update_on_gpu() {
  if (!on_gpu_) {
    return;
  }

  boost::shared_array<float> attrib_array = to_array();
  glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);
  glBufferSubData(GL_ARRAY_BUFFER, 0, size_array_*sizeof(float), attrib_array.get());
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

boost::shared_array<float> Mesh::to_array() {

  boost::shared_array<float> attrib_array(new float[size_array_]);

  if (type_ == GL_TRIANGLES) {
    for (int i = 0; i < triangle_count_ * 3; ++i) {
      unsigned int vertex_start = i * ELEMENTS_PER_VERTEX;
      Vector3f pos = verticies_.at(i);
      attrib_array[vertex_start] = pos.x();
      attrib_array[vertex_start+1] = pos.y();
      attrib_array[vertex_start+2] = pos.z();
      Vector2f tex = tex_coords_.at(i);
      attrib_array[vertex_start+3] = tex.u();
      attrib_array[vertex_start+4] = tex.v();
      Vector3f norm = normals_.at(i);
      attrib_array[vertex_start+5] = norm.x();
      attrib_array[vertex_start+6] = norm.y();
      attrib_array[vertex_start+7] = norm.z();
    }
  } else if (type_ == GL_POINTS) {
    for (int i = 0; i < verticies_.size(); ++i) {
      unsigned int vertex_start = i * 3;
      Vector3f pos = verticies_.at(i);
      attrib_array[vertex_start] = pos.x();
      attrib_array[vertex_start+1] = pos.y();
      attrib_array[vertex_start+2] = pos.z();
    }
  }

  return attrib_array;
}
