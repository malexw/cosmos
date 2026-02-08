#ifndef COSMOS_MESHMANAGER_H_
#define COSMOS_MESHMANAGER_H_

#include <vector>

#include <memory>

#include "FileBlob.hpp"
#include "Mesh.hpp"
#include "util.hpp"

class MeshManager {
 public:
	typedef std::shared_ptr<MeshManager> ShPtr;

	MeshManager();
	
  static MeshManager& get();
	
	// Iterates through the list of loaded meshes searching for a mesh with the same name as "name".
	// Returns the first mesh found with a matching name. Returns an empty pointer if no matching mesh
	// is found.
	const Mesh::ShPtr get_mesh(const std::string name) const;

  // Loads a mesh from the given path if it hasn't been loaded yet.
  // Returns the existing mesh if already loaded.
  Mesh::ShPtr load_mesh(const std::string& path);

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
  unsigned int newline_index(const FileBlob& b, unsigned int offset) const;
  // Returns a collection of whitespace-separated character strings occuring between offset and the end of the
  // line
  const std::vector<std::string> Tokenize(const FileBlob& b, const unsigned int offset) const;
	
	DISALLOW_COPY_AND_ASSIGN(MeshManager);
};

#endif
