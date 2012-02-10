#ifndef COSMOS_MESH_H_
#define COSMOS_MESH_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include "SDL/SDL.h"
//#include "SDL/SDL_opengl.h"
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"

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

  Mesh(std::string name): name_(name), triangle_count_(0), on_gpu_(false) {}

  // This is to be used by whatever is parsing the OBJ file to build the geometry
  void add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                    Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                    Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3);

  void uploadToGpu();

  // Called by the engine to submit the geometry to the GPU
  void draw() const;

  const bool is_name(const std::string& rhs) const;
  void set_tex_pointer() { glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[0]); }

  // The number of triangles in the mesh
  const unsigned int triangle_count() const { return triangle_count_; }

 private:
  std::string name_;
  unsigned int triangle_count_;
  std::vector<Vector3f> verticies_;
  std::vector<Vector3f> normals_;
  std::vector<Vector2f> tex_coords_;
  // Hopefully one day we won't need a whole new mesh object for color variations
  std::vector<Vector3f> colors_;

  GLuint vbo_address_;
  bool on_gpu_;
  int offsets_[4];

  DISALLOW_COPY_AND_ASSIGN(Mesh);
};

#endif
