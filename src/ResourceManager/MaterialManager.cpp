#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "SDL/SDL_image.h"

#include "MaterialManager.hpp"
#include "TextureManager.hpp"

MaterialManager::MaterialManager(TextureManager::ShPtr texture_manager)
  : texture_manager_(texture_manager) {
  init();
}

void MaterialManager::init() {
}

const Material::ShPtr MaterialManager::get_material(const std::string& path) {
  Material::ShPtr ret;
  MaterialTable::iterator iter = materials_.find(path);

  if (iter != materials_.end()) {
    ret = iter->second;
  } else {
    if (load_material(path)) {
      ret = materials_.find(path)->second;
    } else {
      // ret = Material::DEFAULT;
      ret = Material::ShPtr();
    }
  }

  return ret;
}

const bool MaterialManager::load_material(const std::string& path) {
  // For now, assuming one material per .mat file.
  FileBlob::ShPtr file(new FileBlob(path));
  bool ret = file->is_valid();

  if (ret) {
    materials_.insert(MaterialTable::value_type(path, decode(*file)));
  } else {
    std::cout << "Error: material file <" << path << "> not found" << std::endl;
  }

  return ret;
}

Material::ShPtr MaterialManager::decode(FileBlob& b) {

  int index = 0;

  //std::vector<Vector3f> verts;
  //std::vector<Vector2f> uvs;
  //std::vector<Vector3f> norms;
  Material::ShPtr mat(new Material(b.path()));

  std::vector<std::string> tokens;

  while (index < b.size()) {
    tokens = Tokenize(b, index);

    if (tokens.size() > 0) {
      if (tokens[0] == "#") {
        // this line is a comment - skip it
      } else if (tokens[0] == "newmtl") {
        // the name of the material is tokens[1]
        mat->set_name(tokens[1]);
      } else if (tokens[0] == "Ka") {
        // Ambient lighting color
        /*float r = boost::lexical_cast<float>(tokens[1]);
        float g = boost::lexical_cast<float>(tokens[2]);
        float b = boost::lexical_cast<float>(tokens[2]);*/
      } else if (tokens[0] == "Kd") {
        // Diffuse lighting color
        float r = boost::lexical_cast<float>(tokens[1]);
        float g = boost::lexical_cast<float>(tokens[2]);
        float b = boost::lexical_cast<float>(tokens[2]);
        mat->set_diff_color(Vector3f(r, g, b));
      } else if (tokens[0] == "Ks") {
        // Specular lighting color
        /*float r = boost::lexical_cast<float>(tokens[1]);
        float g = boost::lexical_cast<float>(tokens[2]);
        float b = boost::lexical_cast<float>(tokens[2]);*/
      } else if (tokens[0] == "Ns") {
        // Shininess
        float s = boost::lexical_cast<float>(tokens[1]);
      } else if (tokens[0] == "d") {
        // dissolve, the .OBJ's version of alpha
      } else if (tokens[0] == "illum") {
        // Lighting model
        //int model = boost::lexical_cast<int>(tokens[1]) - 1;
      } else if (tokens[0] == "map_Ka") {
        // ambient texture map
      } else if (tokens[0] == "map_Kd") {
        // diffuse texture map
        mat->set_texture(texture_manager_->get_texture("res/textures/" + tokens[1]));
      } else if (tokens[0] == "map_Ks") {
        // specular texture map
      } else if (tokens[0] == "map_d") {
        // dissolve texture map
      } else if (tokens[0] == "map_bump") {
        mat->set_bump_tex(texture_manager_->get_texture("res/textures/" + tokens[1]));
        //mat->set_n11n_tex(TextureManager::get().get_texture("normalization_map");
        // bump map
      } else {
        std::cout << ".mtl processing warning: unknown token <" << tokens[0] << ">" << std::endl;
      }

      index = newline_index(b, index+1);
    } else {
      break;
    }
  }

  return mat;
}

// Returns the index of the first character following a group of newline characters after the offset
const unsigned int MaterialManager::newline_index(const FileBlob& b, const unsigned int offset) const {

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
const std::vector<std::string> MaterialManager::Tokenize(const FileBlob& b, const unsigned int offset) const {
  std::vector<std::string> tokens;

  std::string line;
  line.assign(&b[offset], &b[newline_index(b, offset)]);
  boost::split(tokens, line, boost::is_any_of("\t /\r\n"));

  return tokens;
}
