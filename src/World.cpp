#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "FileBlob.hpp"
#include "ResourceManager/MaterialManager.hpp"
#include "World.hpp"

World::World(std::string path)
 : path_(path), triangle_count_(0) {
  init();
}

void World::init() {
  FileBlob::ShPtr file(new FileBlob(path_));
  std::cout << "Loading world " << path_ << std::endl;
  decode(*file);
}

void World::add_triangle( Vector3f v1, Vector2f vt1, Vector3f vn1, Vector3f c1,
                          Vector3f v2, Vector2f vt2, Vector3f vn2, Vector3f c2,
                          Vector3f v3, Vector2f vt3, Vector3f vn3, Vector3f c3) {
  verticies_.push_back(v1);
  verticies_.push_back(v2);
  verticies_.push_back(v3);
  
  tex_coords_.push_back(vt1);
  tex_coords_.push_back(vt2);
  tex_coords_.push_back(vt3);
  
  normals_.push_back(vn1);
  normals_.push_back(vn2);
  normals_.push_back(vn3);
  
  colors_.push_back(c1);
  colors_.push_back(c2);
  colors_.push_back(c3);
  
  triangle_count_ += 1;
  mats_.back().second += 1;
}

void World::set_material(Material::ShPtr mat) {
  World::MatPair m(mat, 0);
  mats_.push_back(m);
}

void World::draw() const {
	int drawn = 0;
  foreach (World::MatPair mat, mats_) {
    glBindTexture(GL_TEXTURE_2D, mat.first->get_texture()->get_index());
    glVertexPointer(3, GL_FLOAT, 0, &verticies_[drawn]);
    glTexCoordPointer(2, GL_FLOAT, 0, &tex_coords_[drawn]);
    glNormalPointer(GL_FLOAT, 0, &normals_[drawn]);
    glColorPointer(3, GL_FLOAT, 0, &colors_[drawn]);
    glDrawArrays(GL_TRIANGLES, 0, mat.second * 3);
    drawn += mat.second * 3;
  }
}

void World::decode(FileBlob& b) {
   
  int index = 0;
  int triangles = 0;
  
  std::vector<Vector3f> verts;
  std::vector<Vector2f> uvs;
  std::vector<Vector3f> norms;
  
  std::vector<std::string> tokens;
  
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
		  } else if (tokens[0] == "o") {
        // it's an object spawner 
      } else if (tokens[0] == "usemtl") {
        set_material(MaterialManager::get().get_material(std::string("res/materials/") + tokens[1]));
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
			  Vector3f color = mats_.back().first->get_diff_color();
        //std::cout << x << " " << y << " " << z << " " << n << std::endl;
			  add_triangle( verts[v1i], uvs[vt1i], norms[vn1i], color,
                      verts[v2i], uvs[vt2i], norms[vn2i], color,
                      verts[v3i], uvs[vt3i], norms[vn3i], color);
		  }
		  index = newline_index(b, index+1);
	  } else {
		  break;
	  }
  }
  
  std::cout << "Read " << triangle_count() << " triangles" << std::endl;
}

// Returns the index of the first character following a group of newline characters after the offset
const unsigned int World::newline_index(const FileBlob& b, const unsigned int offset) const {
  
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
const std::vector<std::string> World::Tokenize(const FileBlob& b, const unsigned int offset) const {
  std::vector<std::string> tokens;
  
  std::string line;
  line.assign(&b[offset], &b[newline_index(b, offset)]);
  boost::split(tokens, line, boost::is_any_of("\t /\r\n"));
  
  return tokens;
}
