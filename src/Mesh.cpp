#include "Mesh.hpp"

#include <cstdint>
#include <memory>

void Mesh::add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                        Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                        Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3,
                        Vector3f tangent, Vector3f bitangent) {

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

  tangents_.push_back(tangent);
  tangents_.push_back(tangent);
  tangents_.push_back(tangent);

  bitangents_.push_back(bitangent);
  bitangents_.push_back(bitangent);
  bitangents_.push_back(bitangent);

  triangle_count_ += 1;
  submeshes_.back().triangle_count += 1;
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
  offsets_[4] = offsets_[3] + colsize;
  int tansize = tangents_.size() * 3 * fsize;
  offsets_[5] = offsets_[4] + tansize;
  int bitansize = bitangents_.size() * 3 * fsize;

  // Create VAO
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  // Create VBO
  glGenBuffers(1, &vbo_address_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);
  glBufferData(GL_ARRAY_BUFFER, vertsize + texsize + normsize + colsize + tansize + bitansize, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[0], vertsize, &verticies_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[1], texsize, &tex_coords_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[2], normsize, &normals_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[3], colsize, &colors_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[4], tansize, &tangents_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, offsets_[5], bitansize, &bitangents_[0]);

  // Generic vertex attributes
  // location 0 = position (vec3)
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsets_[0])));
  // location 1 = texcoord (vec2)
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsets_[1])));
  // location 2 = normal (vec3)
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsets_[2])));
  // location 3 = color (vec3)
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsets_[3])));
  // location 4 = tangent (vec3)
  glEnableVertexAttribArray(4);
  glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsets_[4])));
  // location 5 = bitangent (vec3)
  glEnableVertexAttribArray(5);
  glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(static_cast<std::uintptr_t>(offsets_[5])));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  on_gpu_ = true;
}

void Mesh::bind() const {
  if (on_gpu_) {
    glBindVertexArray(vao_);
  }
}

bool Mesh::is_name(std::string_view rhs) const {
	return name_ == rhs;
}

Mesh::ShPtr Mesh::create_quad() {
    auto mesh = std::make_shared<Mesh>("__quad");
    mesh->begin_submesh(Material::ShPtr());

    Vector3f v0(-0.5f,  0.5f, 0.0f);  // top-left
    Vector3f v1( 0.5f,  0.5f, 0.0f);  // top-right
    Vector3f v2( 0.5f, -0.5f, 0.0f);  // bottom-right
    Vector3f v3(-0.5f, -0.5f, 0.0f);  // bottom-left

    Vector2f uv0(0.0f, 0.0f);
    Vector2f uv1(1.0f, 0.0f);
    Vector2f uv2(1.0f, 1.0f);
    Vector2f uv3(0.0f, 1.0f);

    Vector3f normal(0.0f, 0.0f, 1.0f);
    Vector3f color;
    Vector3f tangent(1.0f, 0.0f, 0.0f);
    Vector3f bitangent(0.0f, 1.0f, 0.0f);

    // Triangle 1: v1, v0, v2  (matches face-center-quad.obj winding)
    mesh->add_triangle(
        v1, uv2, normal, color,
        v0, uv3, normal, color,
        v2, uv1, normal, color,
        tangent, bitangent);

    // Triangle 2: v2, v0, v3
    mesh->add_triangle(
        v2, uv1, normal, color,
        v0, uv3, normal, color,
        v3, uv0, normal, color,
        tangent, bitangent);

    mesh->uploadToGpu();
    return mesh;
}
