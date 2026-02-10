#ifndef COSMOS_RESOURCEMANAGER_H_
#define COSMOS_RESOURCEMANAGER_H_

#include <vector>
#include <unordered_map>

#include <memory>

#include "ParticleEmitterDef.hpp"
#include "Texture.hpp"
#include "util.hpp"

class ResourceManager {
 public:
	using ShPtr = std::shared_ptr<ResourceManager>;

	ResourceManager();

  static ResourceManager& get();

  // Load a resource file. Dispatches by extension and .tres type:
  //   .tres type="Texture" — loads texture via TextureManager
  //   .obj                 — loads mesh via MeshManager
  //   .mtl                 — loads material(s) via MaterialManager
  void load_resource(const std::string& path);

  // Retrieve a previously loaded resource by its .tres path.
  Texture::ShPtr get_texture(const std::string& path) const;

 private:

	void init();

  Texture::ShPtr load_texture_tres(const std::string& tres_path);
  void load_particle_emitter_tres(const std::string& tres_path);

  std::unordered_map<std::string, Texture::ShPtr> textures_;

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;
};

#endif
