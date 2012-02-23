#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <iostream>

//#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "Renderer.hpp"

MeshManager::MeshManager() {
  init();
}

void MeshManager::init() {
  // Create the default mesh
  Mesh::ShPtr default_mesh(new Mesh("__err_mesh", GL_TRIANGLES));
  Vector3f v1(-0.5, 0.5, 0.5); Vector3f v2(0.5, 0.5, 0.5); Vector3f v3(-0.5, -0.5, 0.5); Vector3f v4(0.5, -0.5, 0.5);
  Vector3f v5(-0.5, 0.5, -0.5); Vector3f v6(0.5, 0.5, -0.5); Vector3f v7(-0.5, -0.5, -0.5); Vector3f v8(0.5, -0.5, -0.5);
  Vector2f vt1(0, 0); Vector2f vt2(1, 0); Vector2f vt3(0, 1); Vector2f vt4(1, 1);
  Vector3f vn1(0, 0, 1); Vector3f vn2(1, 0, 0); Vector3f vn3(0, 0, -1); Vector3f vn4(-1, 0, 0); Vector3f vn5(0, 1, 0); Vector3f vn6(0, -1, 0);

  default_mesh->add_triangle(v2, vt2, vn1, v1, vt1, vn1, v4, vt4, vn1);
  default_mesh->add_triangle(v4, vt4, vn1, v1, vt1, vn1, v3, vt3, vn1);
  default_mesh->add_triangle(v4, vt3, vn6, v3, vt4, vn6, v7, vt2, vn6);
  default_mesh->add_triangle(v7, vt3, vn4, v3, vt4, vn4, v1, vt2, vn4);
  default_mesh->add_triangle(v7, vt3, vn4, v1, vt2, vn4, v5, vt1, vn4);
  default_mesh->add_triangle(v5, vt3, vn5, v1, vt4, vn5, v2, vt2, vn5);
  default_mesh->add_triangle(v5, vt3, vn5, v2, vt2, vn5, v6, vt1, vn5);
  default_mesh->add_triangle(v6, vt2, vn2, v2, vt1, vn2, v4, vt3, vn2);
  default_mesh->add_triangle(v6, vt2, vn2, v4, vt3, vn2, v8, vt4, vn2);
  default_mesh->add_triangle(v8, vt1, vn6, v4, vt3, vn6, v7, vt2, vn6);
  default_mesh->add_triangle(v8, vt3, vn3, v7, vt4, vn3, v6, vt1, vn3);
  default_mesh->add_triangle(v6, vt1, vn3, v7, vt4, vn3, v5, vt2, vn3);

  default_mesh->upload_to_gpu();
  meshes_.insert(MeshTable::value_type("__err_mesh", default_mesh));
}

Mesh::ShPtr MeshManager::get_mesh(const std::string& path) {
  Mesh::ShPtr ret;
  MeshTable::iterator iter = meshes_.find(path);

  if (iter != meshes_.end()) {
    ret = iter->second;
  } else {
    if (load_mesh(path)) {
      ret = meshes_.find(path)->second;
    } else {
      // ret = Material::DEFAULT;
      ret = Mesh::ShPtr();
    }
  }

  return ret;
}

const bool MeshManager::load_mesh(const std::string& path) {
  bool ret = false;

  FileBlob::ShPtr file(new FileBlob(path));
  if (file->is_valid()) {
    Mesh::ShPtr new_mesh = decode(*file);
    meshes_.insert(MeshTable::value_type(path, new_mesh));
    ret = true;
  } else {
    std::cout << "Error: unable to load mesh <" << path << ">" << std::endl;
  }

  return ret;
}

Mesh::ShPtr MeshManager::decode(FileBlob& b) {

  int index = 0;
  int triangles = 0;

  std::vector<Vector3f> verts;
  std::vector<Vector2f> uvs;
  std::vector<Vector3f> norms;

  std::vector<std::string> tokens;
  Mesh::ShPtr mesh (new Mesh(b.path(), GL_TRIANGLES));
  //Material::ShPtr mat;

  while (index < b.size()) {
    tokens = Tokenize(b, index);

    if (tokens.size() > 0) {
      if (tokens[0] == "#") {
        // this line is a comment - skip it
      } else if (tokens[0] == "v") {
        // found a vertex
        float x = boost::lexical_cast<float>(tokens[1]);
        float y = boost::lexical_cast<float>(tokens[2]);
        float z = boost::lexical_cast<float>(tokens[3]);
        verts.push_back(Vector3f(x, y, z));
      } else if (tokens[0] == "vt") {
        // found a normal
        float u = boost::lexical_cast<float>(tokens[1]);
        float v = boost::lexical_cast<float>(tokens[2]);
        uvs.push_back(Vector2f(u, v));
      } else if (tokens[0] == "vn") {
        // found a normal
        float x = boost::lexical_cast<float>(tokens[1]);
        float y = boost::lexical_cast<float>(tokens[2]);
        float z = boost::lexical_cast<float>(tokens[3]);
        norms.push_back(Vector3f(x, y, z));
      } else if (tokens[0] == "usemtl") {
        // Do nothing here. Though the OBJ format supports specifying the material at a per-face level, this engine
        // assumes one material per mesh, which is specified at run time by the game script.
        //mat = (MaterialManager::get().get_material("res/materials/" + tokens[1]));
        //std::cout << "Material has color " << mat->get_diff_color() << std::endl;
      } else if (tokens[0] == "f") {
        // -1 to each of these because OBJ uses 1-based indexing
        int v1i = boost::lexical_cast<int>(tokens[1]) - 1;
        int vt1i = boost::lexical_cast<int>(tokens[2]) - 1;
        int vn1i = boost::lexical_cast<int>(tokens[3]) - 1;
        int v2i = boost::lexical_cast<int>(tokens[4]) - 1;
        int vt2i = boost::lexical_cast<int>(tokens[5]) - 1;
        int vn2i = boost::lexical_cast<int>(tokens[6]) - 1;
        int v3i = boost::lexical_cast<int>(tokens[7]) - 1;
        int vt3i = boost::lexical_cast<int>(tokens[8]) - 1;
        int vn3i = boost::lexical_cast<int>(tokens[9]) - 1;

        mesh->add_triangle( verts[v1i], uvs[vt1i], norms[vn1i],
                            verts[v2i], uvs[vt2i], norms[vn2i],
                            verts[v3i], uvs[vt3i], norms[vn3i]);
      }
      index = newline_index(b, index+1);
    } else {
      break;
    }
  }

  mesh->upload_to_gpu();

  return mesh;
}

// Returns the index of the first character following a group of newline characters after the offset
const unsigned int MeshManager::newline_index(const FileBlob& b, const unsigned int offset) const {

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
  std::vector<std::string> tokens;

  std::string line;
  line.assign(&b[offset], &b[newline_index(b, offset)]);
  boost::split(tokens, line, boost::is_any_of("\t /\r\n"));

  return tokens;
}
