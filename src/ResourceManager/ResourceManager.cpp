#include <iostream>
#include <fstream>
#include <string>

#include <sstream>

#include "AudioManager.hpp"
#include "FontManager.hpp"
#include "MaterialManager.hpp"
#include "MeshManager.hpp"
#include "ParticleManager.hpp"
#include "ResourceManager.hpp"
#include "ShaderManager.hpp"
#include "TextureManager.hpp"

ResourceManager::ResourceManager() {
	init();
}

void ResourceManager::init() {
  TextureManager::get();
  FontManager::get();
  MaterialManager::get();
  MeshManager::get();
  ShaderManager::get();
  AudioManager::get();
  ParticleManager::get();
}

/*
 * Singleton pattern
 */
ResourceManager& ResourceManager::get() {
  static ResourceManager instance;
  return instance;
}

static std::string trim(const std::string& s) {
  size_t start = s.find_first_not_of(" \t");
  if (start == std::string::npos) return "";
  size_t end = s.find_last_not_of(" \t");
  return s.substr(start, end - start + 1);
}

static std::string strip_quotes(const std::string& s) {
  if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
    return s.substr(1, s.size() - 2);
  }
  return s;
}

static std::string read_tres_type(const std::string& path) {
  std::ifstream file(path);
  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();
    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#') continue;
    if (trimmed[0] == '[') {
      auto pos = trimmed.find("type=\"");
      if (pos != std::string::npos) {
        pos += 6;
        auto end = trimmed.find('"', pos);
        if (end != std::string::npos) return trimmed.substr(pos, end - pos);
      }
    }
    return "";
  }
  return "";
}

void ResourceManager::load_resource(const std::string& path) {
  size_t dot = path.rfind('.');
  if (dot == std::string::npos) {
    std::cout << "ResourceManager: no extension in " << path << std::endl;
    return;
  }
  std::string ext = path.substr(dot + 1);

  if (ext == "tres") {
    std::string type = read_tres_type(path);
    if (type == "Texture") {
      Texture::ShPtr tex = load_texture_tres(path);
      if (tex) textures_[path] = tex;
    } else if (type == "ParticleEmitter") {
      load_particle_emitter_tres(path);
    } else {
      std::cout << "ResourceManager: unknown resource type \"" << type << "\" in " << path << std::endl;
    }
  } else if (ext == "obj") {
    MeshManager::get().load_mesh(path);
  } else if (ext == "mtl") {
    MaterialManager::get().load_material(path);
  } else {
    std::cout << "ResourceManager: unknown extension ." << ext << " in " << path << std::endl;
  }
}

Texture::ShPtr ResourceManager::get_texture(const std::string& path) const {
  auto it = textures_.find(path);
  if (it != textures_.end()) return it->second;
  return Texture::ShPtr();
}

Texture::ShPtr ResourceManager::load_texture_tres(const std::string& tres_path) {
  std::ifstream file(tres_path);
  if (!file.is_open()) {
    std::cout << "ResourceManager: could not open " << tres_path << std::endl;
    return Texture::ShPtr();
  }

  std::string source;
  TexFilter filter = TexFilter::Linear;
  TexWrap wrap = TexWrap::Repeat;
  float exposure = -1.0f;
  bool header_found = false;

  std::string line;
  while (std::getline(file, line)) {
    // Strip trailing \r for CRLF files
    if (!line.empty() && line.back() == '\r') line.pop_back();

    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#') continue;

    // Header line
    if (trimmed[0] == '[') {
      if (trimmed.find("type=\"Texture\"") != std::string::npos) {
        header_found = true;
      }
      continue;
    }

    if (!header_found) continue;

    // Parse key = value
    size_t eq = trimmed.find('=');
    if (eq == std::string::npos) continue;

    std::string key = trim(trimmed.substr(0, eq));
    std::string value = strip_quotes(trim(trimmed.substr(eq + 1)));

    if (key == "source") {
      source = value;
    } else if (key == "filter") {
      if (value == "nearest") filter = TexFilter::Nearest;
      else filter = TexFilter::Linear;
    } else if (key == "wrap") {
      if (value == "clamp") wrap = TexWrap::Clamp;
      else if (value == "mirror") wrap = TexWrap::Mirror;
      else wrap = TexWrap::Repeat;
    } else if (key == "exposure") {
      exposure = std::stof(value);
    }
  }

  if (source.empty()) {
    std::cout << "ResourceManager: no source specified in " << tres_path << std::endl;
    return Texture::ShPtr();
  }

  Texture::ShPtr tex = TextureManager::get().load_texture(source, filter, wrap);
  if (tex && exposure >= 0.0f) {
    tex->set_default_exposure(exposure);
  }
  return tex;
}

void ResourceManager::load_particle_emitter_tres(const std::string& tres_path) {
  std::ifstream file(tres_path);
  if (!file.is_open()) {
    std::cout << "ResourceManager: could not open " << tres_path << std::endl;
    return;
  }

  ParticleEmitterDef::ShPtr def(new ParticleEmitterDef());
  bool header_found = false;

  std::string line;
  while (std::getline(file, line)) {
    if (!line.empty() && line.back() == '\r') line.pop_back();

    std::string trimmed = trim(line);
    if (trimmed.empty() || trimmed[0] == '#') continue;

    if (trimmed[0] == '[') {
      if (trimmed.find("type=\"ParticleEmitter\"") != std::string::npos) {
        header_found = true;
      }
      continue;
    }

    if (!header_found) continue;

    size_t eq = trimmed.find('=');
    if (eq == std::string::npos) continue;

    std::string key = trim(trimmed.substr(0, eq));
    std::string value = strip_quotes(trim(trimmed.substr(eq + 1)));

    if (key == "mesh") {
      def->mesh_path = value;
    } else if (key == "texture") {
      def->texture_path = value;
    } else if (key == "emission_rate") {
      def->emission_rate = std::stof(value);
    } else if (key == "lifetime") {
      def->lifetime = std::stof(value);
    } else if (key == "speed") {
      def->speed = std::stof(value);
    } else if (key == "spread") {
      def->spread = std::stof(value);
    } else if (key == "start_color") {
      std::istringstream ss(value);
      ss >> def->start_color.r >> def->start_color.g >> def->start_color.b >> def->start_color.a;
    } else if (key == "end_color") {
      std::istringstream ss(value);
      ss >> def->end_color.r >> def->end_color.g >> def->end_color.b >> def->end_color.a;
    } else if (key == "start_size") {
      def->start_size = std::stof(value);
    } else if (key == "end_size") {
      def->end_size = std::stof(value);
    } else if (key == "gravity") {
      std::istringstream ss(value);
      ss >> def->gravity.x >> def->gravity.y >> def->gravity.z;
    } else if (key == "blend_mode") {
      if (value == "additive") def->blend_mode = ParticleBlendMode::Additive;
      else def->blend_mode = ParticleBlendMode::Alpha;
    }
  }

  // Load sub-resources
  if (!def->mesh_path.empty()) load_resource(def->mesh_path);
  if (!def->texture_path.empty()) load_resource(def->texture_path);

  ParticleManager::get().store_emitter_def(tres_path, def);
}

