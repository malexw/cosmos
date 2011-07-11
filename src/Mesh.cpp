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

void Mesh::draw() const {
	glVertexPointer(3, GL_FLOAT, 0, &verticies_[0]);
	glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[0]);
	glNormalPointer(GL_FLOAT, 0, &normals_[0]);
  glColorPointer(3, GL_FLOAT, 0, &colors_[0]);
}

const bool Mesh::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}
