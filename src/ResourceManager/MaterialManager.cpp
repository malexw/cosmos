#include <iostream>

#include <string>

#include "MaterialManager.hpp"
#include "ResourceManager.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

MaterialManager::MaterialManager() 
  : loaded_(false) {
	init();
}

/*
 * This initialization function is just to make it easier to manually edit the 'to-be-loaded' textures list.
 * In the future, TextureMan should read from some kind of resource file so we don't need to specify these by
 * hand
 */
void MaterialManager::init() {
  load_materials();
}

/*
 * Singleton pattern
 */
MaterialManager& MaterialManager::get() {
  static MaterialManager instance;
  return instance;
}

Material::ShPtr MaterialManager::load_material(const std::string& path) {
  // Return existing material if already loaded
  for (const Material::ShPtr& mat : mats_) {
    if (mat->is_name(path)) {
      return mat;
    }
  }

  std::cout << "Decoding " << path << std::endl;
  FileBlob::ShPtr file(new FileBlob(path));
  Material::ShPtr mat = decode(*file);
  mats_.push_back(mat);
  return mat;
}

void MaterialManager::load_materials() {
	if (loaded_) {
		std::cout << "MaterialMan: Error - materials already loaded" << std::endl;
		return;
	}

  for (unsigned int j = 0; j < mat_names_.size(); ++j) {
    load_material(mat_names_[j]);
	}

	loaded_ = true;
}

/*
 * Uses a dumb linear search to find a texture with the same name. Optimizations welcome!
 */
const Material::ShPtr MaterialManager::get_material(std::string name) const {
	for (const Material::ShPtr& mat : mats_) {
		if (mat->is_name(name)) {
			return mat;
		}
	}
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
        float r = std::stof(tokens[1]);
			  float g = std::stof(tokens[2]);
        float b = std::stof(tokens[3]);
        mat->set_ambient_color(Vector3f(r, g, b));
		  } else if (tokens[0] == "Kd") {
			  // Diffuse lighting color
        float r = std::stof(tokens[1]);
			  float g = std::stof(tokens[2]);
        float b = std::stof(tokens[3]);
        mat->set_diffuse_color(Vector3f(r, g, b));
		  } else if (tokens[0] == "Ks") {
        // Specular lighting color
        float r = std::stof(tokens[1]);
			  float g = std::stof(tokens[2]);
        float b = std::stof(tokens[3]);
        mat->set_specular_color(Vector3f(r, g, b));
      } else if (tokens[0] == "Ns") {
        // Shininess
        // float s = std::stof(tokens[1]);
      } else if (tokens[0] == "d") {
        // dissolve, the .OBJ's version of alpha
      } else if (tokens[0] == "illum") {
			  // Lighting model
			  //int model = boost::lexical_cast<int>(tokens[1]) - 1;
		  } else if (tokens[0] == "map_Ka") {
        // ambient texture map
      } else if (tokens[0] == "map_Kd") {
        // diffuse texture map
        std::string tex_path = "res/textures/" + tokens[1];
        ResourceManager::get().load_resource(tex_path);
        mat->set_texture(ResourceManager::get().get_texture(tex_path));
      } else if (tokens[0] == "map_Ks") {
        // specular texture map
      } else if (tokens[0] == "map_d") {
        // dissolve texture map
        std::string tex_path = "res/textures/" + tokens[1];
        ResourceManager::get().load_resource(tex_path);
        mat->set_decal_tex(ResourceManager::get().get_texture(tex_path));
      } else if (tokens[0] == "map_bump") {
        std::string tex_path = "res/textures/" + tokens[1];
        ResourceManager::get().load_resource(tex_path);
        mat->set_bump_tex(ResourceManager::get().get_texture(tex_path));
      } else {
        std::cout << ".mtl processing warning: unknown token <" << tokens[0] << ">" << std::endl;
      }

      index = newline_index(b, index+1);
    } else {
      break;
    }
  }

  if (mat->is_textured()) {
    mat->set_shader(ShaderManager::get().get_shader_program("bumpdec"));
  } else {
    mat->set_shader(ShaderManager::get().get_shader_program("blinn"));
  }

  return mat;
}

void MaterialManager::load_mtl_library(const std::string& path) {
  FileBlob::ShPtr file(new FileBlob(path));
  std::cout << "Loading MTL library " << path << std::endl;

  int index = 0;
  Material::ShPtr mat;
  std::vector<std::string> tokens;

  auto finalize_mat = [this](Material::ShPtr& m) {
    if (!m) return;
    if (m->is_textured()) {
      m->set_shader(ShaderManager::get().get_shader_program("bumpdec"));
    } else {
      m->set_shader(ShaderManager::get().get_shader_program("blinn"));
    }
    mats_.push_back(m);
  };

  while (index < file->size()) {
    tokens = Tokenize(*file, index);

    if (tokens.size() > 0) {
      if (tokens[0] == "#") {
        // comment
      } else if (tokens[0] == "newmtl") {
        finalize_mat(mat);
        mat = Material::ShPtr(new Material(tokens[1]));
      } else if (tokens[0] == "Ka") {
        float r = std::stof(tokens[1]);
        float g = std::stof(tokens[2]);
        float b = std::stof(tokens[3]);
        mat->set_ambient_color(Vector3f(r, g, b));
      } else if (tokens[0] == "Kd") {
        float r = std::stof(tokens[1]);
        float g = std::stof(tokens[2]);
        float b = std::stof(tokens[3]);
        mat->set_diffuse_color(Vector3f(r, g, b));
      } else if (tokens[0] == "Ks") {
        float r = std::stof(tokens[1]);
        float g = std::stof(tokens[2]);
        float b = std::stof(tokens[3]);
        mat->set_specular_color(Vector3f(r, g, b));
      } else if (tokens[0] == "map_Kd") {
        std::string tex_path = "res/textures/" + tokens[1];
        ResourceManager::get().load_resource(tex_path);
        mat->set_texture(ResourceManager::get().get_texture(tex_path));
      } else if (tokens[0] == "map_d") {
        std::string tex_path = "res/textures/" + tokens[1];
        ResourceManager::get().load_resource(tex_path);
        mat->set_decal_tex(ResourceManager::get().get_texture(tex_path));
      } else if (tokens[0] == "map_bump") {
        std::string tex_path = "res/textures/" + tokens[1];
        ResourceManager::get().load_resource(tex_path);
        mat->set_bump_tex(ResourceManager::get().get_texture(tex_path));
      }

      index = newline_index(*file, index+1);
    } else {
      break;
    }
  }

  finalize_mat(mat);
}

// Returns the index of the first character following a group of newline characters after the offset
unsigned int MaterialManager::newline_index(const FileBlob& b, unsigned int offset) const {
  
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
  std::string line;
  line.assign(&b[offset], &b[newline_index(b, offset)]);
  return split(line, "\t /\r\n");
}
