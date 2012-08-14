#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "CosmosConfig.hpp"
#include "FileBlob.hpp"
#include "ResourceManager/MaterialManager.hpp"
#include "ResourceManager/TextureManager.hpp"
#include "ResourceManager/ShaderManager.hpp"
#include "World.hpp"

World::World(CosmosSimulation& sim, std::string path)
 : sim_(sim), path_(path), triangle_count_(0), size_array_(0) {
  init();
}

void World::init() {
  FileBlob::ShPtr file(new FileBlob(path_));
  std::cout << "Loading world " << path_ << std::endl;
  //decode(*file);

  upload_to_gpu();

  skybox_material_ = sim_.material_manager_->get_material("res/materials/skybox.mtl");
  skybox_mesh_ = sim_.mesh_manager_->get_mesh("res/meshes/skybox.obj");
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
  size_array_ += 3 * 8;
}

void World::set_material(Material::ShPtr mat) {
  World::MatPair m(mat, 0);
  mats_.push_back(m);
}

void World::draw() const {
  if (CosmosConfig::get().is_textures()) {
    int drawn = 0;
    glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);

    foreach (World::MatPair mat, mats_) {
      mat.first->apply();
      //glBindTexture(GL_TEXTURE_2D, sim_.texture_manager_->get_texture("hdr target")->get_index());
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
      glEnableVertexAttribArray(2);

      glVertexAttribPointer(0, 3/*position x, y, z*/, GL_FLOAT, GL_TRUE, 32/*stride*/, 0/*start*/);
      glVertexAttribPointer(1, 2/*tex u, v*/, GL_FLOAT, GL_TRUE, 32/*stride*/, (void*)12/*start*/);
      glVertexAttribPointer(2, 3/*normal x, y, z*/, GL_FLOAT, GL_TRUE, 32/*stride*/, (void*)20/*start*/);
      glDrawArrays(GL_TRIANGLES, 0, mat.second * 3);

      glDisableVertexAttribArray(0);
      glDisableVertexAttribArray(1);
      glDisableVertexAttribArray(2);

      mat.first->tidy();
    }
    //
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    //
  } else {
    //draw_geometry();
  }
}

void World::draw_geometry() const {
  int drawn = 0;
  glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);

  foreach (World::MatPair mat, mats_) {
    //mat.first->apply();
    //glBindTexture(GL_TEXTURE_2D, sim_.texture_manager_->get_texture("hdr target")->get_index());
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(0, 3/*position x, y, z*/, GL_FLOAT, GL_TRUE, 32/*stride*/, 0/*start*/);
    glVertexAttribPointer(1, 2/*tex u, v*/, GL_FLOAT, GL_TRUE, 32/*stride*/, (void*)12/*start*/);
    glVertexAttribPointer(2, 3/*normal x, y, z*/, GL_FLOAT, GL_TRUE, 32/*stride*/, (void*)20/*start*/);
    glDrawArrays(GL_TRIANGLES, 0, mat.second * 3);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    //mat.first->tidy();
  }
  //
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void World::draw_skybox() const {
  if (!CosmosConfig::get().is_textures()) {
    return;
  }

  skybox_material_->apply();
  skybox_mesh_->draw();
  skybox_material_->tidy();
}

void World::upload_to_gpu() {

  boost::shared_array<float> attrib_array = to_array();

  glGenBuffers(1, &vbo_address_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_address_);
  glBufferData(GL_ARRAY_BUFFER, size_array_*sizeof(float), attrib_array.get(), GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

boost::shared_array<float> World::to_array() {

  boost::shared_array<float> attrib_array(new float[size_array_]);

  for (int i = 0; i < triangle_count_ * 3; ++i) {
    unsigned int vertex_start = i * 8; // 8 elements per vertex;
    Vector3f pos = verticies_.at(i);
    attrib_array[vertex_start] = pos.x();
    attrib_array[vertex_start+1] = pos.y();
    attrib_array[vertex_start+2] = pos.z();
    Vector2f tex = tex_coords_.at(i);
    attrib_array[vertex_start+3] = tex.u();
    attrib_array[vertex_start+4] = tex.v();
    Vector3f norm = normals_.at(i);
    attrib_array[vertex_start+5] = norm.x();
    attrib_array[vertex_start+6] = norm.y();
    attrib_array[vertex_start+7] = norm.z();
  }

  return attrib_array;
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
        set_material(sim_.material_manager_->get_material(std::string("res/materials/") + tokens[1]));
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
