#ifndef COSMOS_MESHMANAGER_H_
#define COSMOS_MESHMANAGER_H_

#include <vector>

#include <boost/shared_ptr.hpp>

#include "FileBlob.hpp"
#include "Mesh.hpp"
#include "util.hpp"

class MeshManager {
 public:
  typedef boost::shared_ptr<MeshManager> ShPtr;

  MeshManager();
  
  // Iterates through the list of loaded textures searching for a texture with the same name as "name".
  // Returns the first texture found with a matching name. Returns an empty pointer if no matching texture
  // is found.
  const Mesh::ShPtr get_mesh(const std::string name) const;

 private:
  bool loaded_;
  std::vector<std::string> mesh_names_;
  std::vector<Mesh::ShPtr> meshes_;
  
  void init();
  
  // Iterates through the list of textures that need to be loaded and loads them. First checks to see if
  // textures have been loaded to prevent duplicate loadings
  void load_meshes();

  // OBJ Decoding stuff -------------
  Mesh::ShPtr decode(FileBlob& b);
  // Returns the index of the first character following a group of newline characters after the offset
  const unsigned int newline_index(const FileBlob& b, const unsigned int offset) const;
  // Returns a collection of whitespace-separated character strings occuring between offset and the end of the
  // line
  const std::vector<std::string> Tokenize(const FileBlob& b, const unsigned int offset) const;
  
  DISALLOW_COPY_AND_ASSIGN(MeshManager);
};

#endif
