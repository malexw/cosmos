#include "Mesh.hpp"

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

  glGenBuffers(1, &vbo_address_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);
  glBufferData(GL_ARRAY_BUFFER, vertsize + texsize + normsize + colsize, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[0], vertsize, &verticies_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[1], texsize, &tex_coords_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[2], normsize, &normals_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[3], colsize, &colors_[0]);

  glVertexPointer(3, GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[0]));
  glTexCoordPointer(2, GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[1]));
  glNormalPointer(GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[2]));
  glColorPointer(3, GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[3]));

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  on_gpu_ = true;
}

void Mesh::draw() const {
  if (on_gpu_) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);

    glClientActiveTexture(GL_TEXTURE0);
    glVertexPointer(3, GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[0]));
    glTexCoordPointer(2, GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[1]));
    glNormalPointer(GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[2]));
    glColorPointer(3, GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[3]));

    //glClientActiveTexture(GL_TEXTURE1);
    //glTexCoordPointer(2, GL_FLOAT, 0, reinterpret_cast<GLbyte*>(offsets_[1]));

    glDrawArrays(GL_TRIANGLES, 0, triangle_count_ * 3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
  } else {
    glClientActiveTexture(GL_TEXTURE0);
    glVertexPointer(3, GL_FLOAT, 0, &verticies_[0]);
    glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[0]);
    glNormalPointer(GL_FLOAT, 0, &normals_[0]);
    glColorPointer(3, GL_FLOAT, 0, &colors_[0]);

    //glClientActiveTexture(GL_TEXTURE1);
    //glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[0]);

    glDrawArrays(GL_TRIANGLES, 0, triangle_count_ * 3);
  }
}

const bool Mesh::is_name(const std::string& rhs) const {
  return name_.compare(rhs) == 0;
}
