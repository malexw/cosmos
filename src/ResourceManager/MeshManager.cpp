#include <iostream>
#include <string>

#include "MaterialManager.hpp"
#include "MeshManager.hpp"

MeshManager::MeshManager() 
  : loaded_(false) {
	init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' font list.
 * In the future, FontMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void MeshManager::init() {
	mesh_names_.push_back(std::string("res/meshes/cube.obj"));
  mesh_names_.push_back(std::string("res/meshes/face-center-quad.obj"));
  mesh_names_.push_back(std::string("res/meshes/edge-center-quad.obj"));
  mesh_names_.push_back(std::string("res/meshes/skybox.obj"));
  mesh_names_.push_back(std::string("res/meshes/hdrbox.obj"));
  load_meshes();
}

/*
 * Singleton pattern
 */
MeshManager& MeshManager::get() {
  static MeshManager instance;
  return instance;
}

/*
 * 
 */
void MeshManager::load_meshes() {
  if (loaded_) {
		std::cout << "FontMan: Error - fonts already loaded" << std::endl;
		return;
	}
    
  int mesh_count = mesh_names_.size();
    
  for (int j = 0; j < mesh_count; ++j) {
    FileBlob::ShPtr file(new FileBlob(mesh_names_[j]));
    std::cout << "Decoding " << file->path() << std::endl;
    meshes_.push_back(decode(*file));
	}
}

/*
 * Uses a dumb linear search to find a font with the same name. Optimizations welcome!
 */
const Mesh::ShPtr MeshManager::get_mesh(std::string name) const {
	for (const Mesh::ShPtr& mesh : meshes_) {
		if (mesh->is_name(name)) {
			return mesh;
		}
	}
	
  std::cout << "Error: mesh <" << name << "> not found" << std::endl;
	return Mesh::ShPtr();
}

Mesh::ShPtr MeshManager::decode(FileBlob& b) {
   
  int index = 0;
  int triangles = 0;
  
  std::vector<Vector3f> verts;
  std::vector<Vector2f> uvs;
  std::vector<Vector3f> norms;
  
  std::vector<std::string> tokens;
  Mesh::ShPtr mesh (new Mesh(b.path()));
  Material::ShPtr mat;
  
  while (index < b.size()) {
	  tokens = Tokenize(b, index);
	  
	  if (tokens.size() > 0) {
		  if (tokens[0] == "#") {
			  // this line is a comment - skip it
		  } else if (tokens[0] == "v") {
			  // found a vertex
			  float x = std::stof(tokens[1]);
			  float y = std::stof(tokens[2]);
			  float z = std::stof(tokens[3]);
			  verts.push_back(Vector3f(x, y, z));
		  } else if (tokens[0] == "vt") {
			  // found a normal
        float u = std::stof(tokens[1]);
			  float v = std::stof(tokens[2]);
			  uvs.push_back(Vector2f(u, v));
		  } else if (tokens[0] == "vn") {
			  // found a normal
        float x = std::stof(tokens[1]);
			  float y = std::stof(tokens[2]);
			  float z = std::stof(tokens[3]);
			  norms.push_back(Vector3f(x, y, z));
		  } else if (tokens[0] == "usemtl") {
        mat = (MaterialManager::get().get_material("res/materials/" + tokens[1]));
        std::cout << "Material has color " << mat->get_diff_color() << std::endl;
      } else if (tokens[0] == "f") {
			  // -1 to each of these because OBJ uses 1-based indexing
			  int v1i = std::stoi(tokens[1]) - 1;
        int vt1i = std::stoi(tokens[2]) - 1;
        int vn1i = std::stoi(tokens[3]) - 1;
			  int v2i = std::stoi(tokens[4]) - 1;
        int vt2i = std::stoi(tokens[5]) - 1;
			  int vn2i = std::stoi(tokens[6]) - 1;
        int v3i = std::stoi(tokens[7]) - 1;
			  int vt3i = std::stoi(tokens[8]) - 1;
        int vn3i = std::stoi(tokens[9]) - 1;
			  //std::cout << x << " " << y << " " << z << " " << n << std::endl;
        Vector3f color = mat->get_diff_color();
			  mesh->add_triangle( verts[v1i], uvs[vt1i], norms[vn1i], color,
                            verts[v2i], uvs[vt2i], norms[vn2i], color,
                            verts[v3i], uvs[vt3i], norms[vn3i], color);
		  }
		  index = newline_index(b, index+1);
	  } else {
		  break;
	  }
  }
  
  //mesh->uploadToGpu();
  std::cout << "Read " << mesh->triangle_count() << " triangles" << std::endl;
  return mesh;
}

// Returns the index of the first character following a group of newline characters after the offset
unsigned int MeshManager::newline_index(const FileBlob& b, unsigned int offset) const {
  
  int ni = offset;
  
  while (!(b[ni] == '\n' || b[ni] == '\r')) {
    ni++;
  }
  while (b[ni] == '\n' || b[ni] == '\r') {
    ni++;
  }
  
  return ni++;
}

// Returns a collection of whitespace-separated character strings occuring between offset and the end of the
// line
const std::vector<std::string> MeshManager::Tokenize(const FileBlob& b, const unsigned int offset) const {
  std::string line;
  line.assign(&b[offset], &b[newline_index(b, offset)]);
  return split(line, "\t /\r\n");
}
