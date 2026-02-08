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
}

/*
 * Singleton pattern
 */
MeshManager& MeshManager::get() {
  static MeshManager instance;
  return instance;
}

Mesh::ShPtr MeshManager::load_mesh(const std::string& path) {
  // Return existing mesh if already loaded
  for (const Mesh::ShPtr& mesh : meshes_) {
    if (mesh->is_name(path)) {
      return mesh;
    }
  }

  FileBlob::ShPtr file(new FileBlob(path));
  std::cout << "Decoding " << file->path() << std::endl;
  Mesh::ShPtr mesh = decode(*file);
  meshes_.push_back(mesh);
  return mesh;
}

void MeshManager::load_meshes() {
  if (loaded_) {
		std::cout << "MeshMan: Error - meshes already loaded" << std::endl;
		return;
	}

  for (unsigned int j = 0; j < mesh_names_.size(); ++j) {
    load_mesh(mesh_names_[j]);
	}

  loaded_ = true;
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
		  } else if (tokens[0] == "mtllib") {
        MaterialManager::get().load_mtl_library("res/materials/" + tokens[1]);
      } else if (tokens[0] == "usemtl") {
        mat = MaterialManager::get().get_material(tokens[1]);
        if (!mat) {
          mat = MaterialManager::get().load_material("res/materials/" + tokens[1]);
        }
        if (!mat) {
          std::cout << "Error: material <" << tokens[1] << "> not found" << std::endl;
        }
        mesh->begin_submesh(mat);
        std::cout << "Material has color " << mat->get_diffuse_color() << std::endl;
      } else if (tokens[0] == "f") {
        // Expects pre-triangulated faces (3 vertices per face)
        int vertex_count = (tokens.size() - 1) / 3;
        if (vertex_count != 3) {
          std::cout << "Warning: " << b.path() << " contains a face with "
                    << vertex_count << " vertices (expected 3). "
                    << "Please triangulate the model before importing." << std::endl;
        }
        int vi[3], vti[3], vni[3];
        for (int i = 0; i < 3; ++i) {
          vi[i]  = std::stoi(tokens[1 + i*3]) - 1;
          vti[i] = std::stoi(tokens[2 + i*3]) - 1;
          vni[i] = std::stoi(tokens[3 + i*3]) - 1;
        }

        Vector3f color = mat->get_diffuse_color();
        Vector3f edge1 = verts[vi[1]] - verts[vi[0]];
        Vector3f edge2 = verts[vi[2]] - verts[vi[0]];
        float du1 = uvs[vti[1]].u() - uvs[vti[0]].u();
        float dv1 = uvs[vti[1]].v() - uvs[vti[0]].v();
        float du2 = uvs[vti[2]].u() - uvs[vti[0]].u();
        float dv2 = uvs[vti[2]].v() - uvs[vti[0]].v();

        float denom = du1 * dv2 - du2 * dv1;
        float r = (denom != 0.0f) ? 1.0f / denom : 0.0f;
        Vector3f tangent   = r * (dv2 * edge1 - dv1 * edge2);
        Vector3f bitangent = r * (du1 * edge2 - du2 * edge1);

        mesh->add_triangle( verts[vi[0]], uvs[vti[0]], norms[vni[0]], color,
                            verts[vi[1]], uvs[vti[1]], norms[vni[1]], color,
                            verts[vi[2]], uvs[vti[2]], norms[vni[2]], color,
                            tangent, bitangent);
		  }
		  index = newline_index(b, index+1);
	  } else {
		  break;
	  }
  }
  
  mesh->uploadToGpu();
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
