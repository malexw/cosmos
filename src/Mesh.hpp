#ifndef COSMOS_MESH_H_
#define COSMOS_MESH_H_

#include <memory>
#include <vector>
#include <string>

#include <SDL2/SDL_opengl.h>

#include "util.hpp"
#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing triangle meshes
 */
class Mesh {
 public:
	typedef std::shared_ptr<Mesh> ShPtr;

	Mesh(std::string name): name_(name), triangle_count_(0), on_gpu_(false), vao_(0), vbo_address_(0) {}

  void add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                    Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                    Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3);

  void uploadToGpu();

  // Bind the VAO (or set up pointers for client-side data)
  void draw() const;

  bool is_name(const std::string& rhs) const;

	unsigned int triangle_count() const { return triangle_count_; }

 private:
  std::string name_;
  unsigned int triangle_count_;
	std::vector<Vector3f> verticies_;
	std::vector<Vector3f> normals_;
	std::vector<Vector2f> tex_coords_;
  std::vector<Vector3f> colors_;

  GLuint vao_;
  GLuint vbo_address_;
  bool on_gpu_;
  int offsets_[4];

  DISALLOW_COPY_AND_ASSIGN(Mesh);
};

#endif
