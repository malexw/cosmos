#ifndef COSMOS_WORLD_H_
#define COSMOS_WORLD_H_

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>

#include "SDL/SDL.h"
#include "Renderer.hpp"

#include "CosmosSimulation.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "util.hpp"
#include "Vector2f.hpp"
#include "Vector3f.hpp"

/*
 * World geometry
 */
class World {
 public:
  typedef boost::shared_ptr<World> ShPtr;

  World(CosmosSimulation& sim, std::string path);
  
  // This is to be used by whatever is parsing the OBJ file to build the geometry
  void add_triangle(Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                    Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                    Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3);
  // Also to be called by whatever is parsing the geometry file
  void set_material(Material::ShPtr mat);
  
  // Called by the engine to submit the geometry to the GPU
  void draw() const;
  void draw_geometry() const;
  void draw_skybox() const;
  
  // The number of triangles in the World
  const unsigned int triangle_count() const { return triangle_count_; }

 private:
  typedef std::pair<Material::ShPtr, int> MatPair;

  CosmosSimulation& sim_;
  Material::ShPtr skybox_material_;
  Mesh::ShPtr skybox_mesh_;

  unsigned int triangle_count_;
  std::vector<Vector3f> verticies_;
  std::vector<Vector3f> normals_;
  std::vector<Vector2f> tex_coords_;
  std::vector<Vector3f> colors_;
  std::vector<MatPair> mats_;
  std::string path_;
  
  void init();
  
  // OBJ Decoding stuff -------------
  void decode(FileBlob& b);
  // Returns the index of the first character following a group of newline characters after the offset
  const unsigned int newline_index(const FileBlob& b, const unsigned int offset) const;
  // Returns a collection of whitespace-separated character strings occuring between offset and the end of the
  // line
  const std::vector<std::string> Tokenize(const FileBlob& b, const unsigned int offset) const;

  
  DISALLOW_COPY_AND_ASSIGN(World);
};

#endif
