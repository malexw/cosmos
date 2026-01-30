#include <string>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include "CosmosConfig.hpp"
#include "FileBlob.hpp"
#include "ResourceManager/MaterialManager.hpp"
#include "ResourceManager/TextureManager.hpp"
#include "ResourceManager/ShaderManager.hpp"
#include "World.hpp"

World::World(std::string path)
 : triangle_count_(0), path_(path), vao_(0), vbo_(0) {
  init();
}

void World::init() {
  FileBlob::ShPtr file(new FileBlob(path_));
  std::cout << "Loading world " << path_ << std::endl;
  decode(*file);
  uploadToGpu();
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

void World::uploadToGpu() {
  int fsize = sizeof(GLfloat);
  int vertsize = verticies_.size() * 3 * fsize;
  int texsize = tex_coords_.size() * 2 * fsize;
  int normsize = normals_.size() * 3 * fsize;
  int colsize = colors_.size() * 3 * fsize;

  int off0 = 0;
  int off1 = vertsize;
  int off2 = off1 + texsize;
  int off3 = off2 + normsize;

  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);

  glGenBuffers(1, &vbo_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, vertsize + texsize + normsize + colsize, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, off0, vertsize, &verticies_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, off1, texsize, &tex_coords_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, off2, normsize, &normals_[0]);
  glBufferSubData(GL_ARRAY_BUFFER, off3, colsize, &colors_[0]);

  // Generic vertex attributes
  // location 0 = position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(off0));
  // location 1 = texcoord
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(off1));
  // location 2 = normal
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(off2));
  // location 3 = color
  glEnableVertexAttribArray(3);
  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, reinterpret_cast<GLvoid*>(off3));

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void World::draw() const {
	if (CosmosConfig::get().is_textures()) {
    int drawn = 0;
    glBindVertexArray(vao_);
    for (const World::MatPair& mat : mats_) {
      glBindTexture(GL_TEXTURE_2D, mat.first->get_texture()->get_index());
      glDrawArrays(GL_TRIANGLES, drawn, mat.second * 3);
      drawn += mat.second * 3;
    }
    glBindVertexArray(0);
  } else {
    draw_geometry();
  }
}

void World::draw_geometry() const {
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLES, 0, triangle_count_ * 3);
  glBindVertexArray(0);
}

void World::decode(FileBlob& b) {
   
  int index = 0;
  
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
		  } else if (tokens[0] == "o") {
        // it's an object spawner 
      } else if (tokens[0] == "usemtl") {
        set_material(MaterialManager::get().get_material(std::string("res/materials/") + tokens[1]));
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
unsigned int World::newline_index(const FileBlob& b, unsigned int offset) const {
  
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
  std::string line;
  line.assign(&b[offset], &b[newline_index(b, offset)]);
  return split(line, "\t /\r\n");
}
