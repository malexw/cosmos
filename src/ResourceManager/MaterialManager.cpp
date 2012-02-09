#include <iostream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "SDL/SDL_image.h"

#include "MaterialManager.hpp"
#include "TextureManager.hpp"

MaterialManager::MaterialManager(TextureManager::ShPtr texture_manager) 
  : texture_manager_(texture_manager), loaded_(false) {
  init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' textures list.
 * In the future, TextureMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void MaterialManager::init() {
  //mat_names_.push_back(std::string("res/textures/default.png"));
  //mat_names_.push_back(std::string("res/textures/terminal.png"));
  mat_names_.push_back(std::string("res/materials/default.mtl"));
  mat_names_.push_back(std::string("res/materials/terminal.mtl"));
  mat_names_.push_back(std::string("res/materials/tronish.mtl"));
  mat_names_.push_back(std::string("res/materials/skybox.mtl"));
  mat_names_.push_back(std::string("res/materials/ion.mtl"));
  mat_names_.push_back(std::string("res/materials/hdrbox.mtl"));
  load_materials();
}

/*
 *
 */
void MaterialManager::load_materials() {
  if (loaded_) {
    std::cout << "TextureMan: Error - textures already loaded" << std::endl;
    return;
  }
  
  int mat_count = mat_names_.size();
    
  for (int j = 0; j < mat_count; ++j) {
    // For now, assuming one material per .mat file.
    //std::cout << "Decoding " << mat_names_[j] << std::endl;
    FileBlob::ShPtr file(new FileBlob(mat_names_[j]));
    mats_.push_back(decode(*file));
  }
  
  loaded_ = true; 
}

/*
 * Uses a dumb linear search to find a texture with the same name. Optimizations welcome!
 */
const Material::ShPtr MaterialManager::get_material(std::string name) const {
  foreach (Material::ShPtr mat, mats_) {
    if (mat->is_name(name)) {
      return mat;
    }
  }
  
  std::cout << "Error: material <" << name << "> not found" << std::endl;
  return Material::ShPtr();
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
