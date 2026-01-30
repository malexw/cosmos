#include "Mesh.hpp"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

void Mesh::add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                        Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                        Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3) {

  verticies_.push_back(v1);
  verticies_.push_back(v2);
  verticies_.push_back(v3);

  tex_coords_.push_back(vt1);
  tex_coords_.push_back(vt2);
  tex_coords_.push_back(vt3);

  normals_.push_back(vn1);
  normals_.push_back(vn2);
  normals_.push_back(vn3);

  colors_.push_back(c1);
  colors_.push_back(c2);
  colors_.push_back(c3);

  triangle_count_ += 1;
}

void Mesh::uploadToGpu() {
  int fsize = sizeof(GLfloat);
  offsets_[0] = 0;
  int vertsize = verticies_.size() * 3 * fsize;
  offsets_[1] = vertsize;
  int texsize = tex_coords_.size() * 2 * fsize;
  offsets_[2] = offsets_[1] + texsize;
  int normsize = normals_.size() * 3 * fsize;
  offsets_[3] = offsets_[2] + normsize;
  int colsize = colors_.size() * 3 * fsize;

  // Create VAO
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  // Create VBO
  glGenBuffers(1, &vbo_address_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);
  glBufferData(GL_ARRAY_BUFFER, vertsize + texsize + normsize + colsize, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[0], vertsize, &verticies_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[1], texsize, &tex_coords_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[2], normsize, &normals_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[3], colsize, &colors_[0]);

  // Generic vertex attributes
  // location 0 = position (vec3)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(offsets_[0]));
  // location 1 = texcoord (vec2)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(offsets_[1]));
  // location 2 = normal (vec3)
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(offsets_[2]));
  // location 3 = color (vec3)
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(offsets_[3]));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  on_gpu_ = true;
}

void Mesh::draw() const {
  if (on_gpu_) {
    glBindVertexArray(vao_);
  }
}

bool Mesh::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}
