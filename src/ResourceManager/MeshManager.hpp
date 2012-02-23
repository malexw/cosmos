#ifndef COSMOS_MESHMANAGER_H_
#define COSMOS_MESHMANAGER_H_

#include <map>
#include <vector>

#include <boost/shared_ptr.hpp>

#include "FileBlob.hpp"
#include "Mesh.hpp"
#include "util.hpp"

class MeshManager {
 public:
  typedef boost::shared_ptr<MeshManager> ShPtr;
  typedef std::map<std::string, Mesh::ShPtr> MeshTable;

  MeshManager();

  /*
   * Looks through the set of loaded meshes for a mesh with the name path. If no loaded mesh is found, attempts to load
   * the mesh pointed at by path. If that fails, returns the default mesh.
   */
  Mesh::ShPtr get_mesh(const std::string& path);

 private:
  MeshTable meshes_;

  void init();

  const bool load_mesh(const std::string& path);

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
