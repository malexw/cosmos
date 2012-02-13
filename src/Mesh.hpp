#ifndef COSMOS_MESH_H_
#define COSMOS_MESH_H_

#include <vector>
#include <string>

#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"
#include "Renderer.hpp"

#include "util.hpp"
#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * A class for representing triangle meshes
 */
class Mesh {
 public:
  typedef boost::shared_ptr<Mesh> ShPtr;

  static const unsigned int ELEMENTS_PER_VERTEX = 8;

  Mesh(std::string name): name_(name), triangle_count_(0), on_gpu_(false), size_array_(0) {}

  // This is to be used by whatever is parsing the OBJ file to build the geometry
  void add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                    Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                    Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3);

  // After deforming vertices, call this to send the updated geometry to the GPU.
  // Does nothing if the geometry hasn't been uploaded to the GPU
  void update_on_gpu();
  
  // Send the geometry to the GPU with the usage pattern GL_STATIC_DRAW
  void upload_to_gpu();
  // Send the geometry to the GPU with a custom usage pattern
  void upload_to_gpu_type(GLenum usage);

  // Called by the engine to submit the geometry to the GPU
  void draw() const;
  
  // Adjust one of the vertices in the mesh relative to its current position
  void deform_relative(unsigned int vertex, Vector3f translation, Vector2f tex_coords, Vector3f normal);
  // Adjust one of the vertices in the mesh by setting its new values
  void deform(unsigned int vertex, Vector3f translation, Vector2f tex_coords, Vector3f normal);

  const bool is_name(const std::string& rhs) const { return name_.compare(rhs) == 0; }
  void set_tex_pointer() { glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[0]); }

  // The number of triangles in the mesh
  const unsigned int triangle_count() const { return triangle_count_; }

 private:
  std::string name_;
  unsigned int triangle_count_;
  unsigned int size_array_;
  std::vector<Vector3f> verticies_;
  std::vector<Vector3f> normals_;
  std::vector<Vector2f> tex_coords_;
  // Hopefully one day we won't need a whole new mesh object for color variations
  std::vector<Vector3f> colors_;

  GLuint vbo_address_;
  bool on_gpu_;
  int offsets_[4];

  void upload_internal(GLenum usage);
  boost::shared_array<float> to_array();

  DISALLOW_COPY_AND_ASSIGN(Mesh);
};

#endif
