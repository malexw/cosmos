#ifndef COSMOS_MESH_H_
#define COSMOS_MESH_H_

#include <memory>
#include <vector>
#include <string>

#include <SDL3/SDL_opengl.h>

#include "Material.hpp"
#include "util.hpp"
#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing triangle meshes
 */
class Mesh {
 public:
	typedef std::shared_ptr<Mesh> ShPtr;

	struct Submesh {
		Material::ShPtr material;
		unsigned int triangle_count;
	};

	Mesh(std::string name): name_(name), triangle_count_(0), vao_(0), vbo_address_(0), on_gpu_(false) {}

  // Create a 1x1 face-centered quad (2 triangles, Z-facing)
  static Mesh::ShPtr create_quad();

  void add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                    Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                    Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3,
                    Vector3f tangent, Vector3f bitangent);

  void uploadToGpu();

  // Bind the VAO
  void bind() const;

  bool is_name(const std::string& rhs) const;

	unsigned int triangle_count() const { return triangle_count_; }
  GLuint vbo() const { return vbo_address_; }
  int vertex_offset() const { return offsets_[0]; }
  int texcoord_offset() const { return offsets_[1]; }
  int normal_offset() const { return offsets_[2]; }
  int color_offset() const { return offsets_[3]; }
  int tangent_offset() const { return offsets_[4]; }
  int bitangent_offset() const { return offsets_[5]; }
  unsigned int vertex_count() const { return triangle_count_ * 3; }

  void begin_submesh(Material::ShPtr mat) { submeshes_.push_back({mat, 0}); }
  const std::vector<Submesh>& submeshes() const { return submeshes_; }

  Mesh& set_material(Material::ShPtr mat) {
    for (auto& sub : submeshes_) { sub.material = mat; }
    return *this;
  }
  Material::ShPtr material() const { return submeshes_.front().material; }

 private:
  std::vector<Submesh> submeshes_;
  std::string name_;
  unsigned int triangle_count_;
	std::vector<Vector3f> verticies_;
	std::vector<Vector3f> normals_;
	std::vector<Vector2f> tex_coords_;
  std::vector<Vector3f> colors_;
  std::vector<Vector3f> tangents_;
  std::vector<Vector3f> bitangents_;

  GLuint vao_;
  GLuint vbo_address_;
  bool on_gpu_;
  int offsets_[6];

  DISALLOW_COPY_AND_ASSIGN(Mesh);
};

#endif
