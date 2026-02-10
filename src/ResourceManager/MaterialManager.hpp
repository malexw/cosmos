#ifndef COSMOS_MATERIALMANAGER_H_
#define COSMOS_MATERIALMANAGER_H_

#include <vector>

#include <memory>

#include "FileBlob.hpp"
#include "Material.hpp"
#include "util.hpp"

class MaterialManager {
 public:
	using ShPtr = std::shared_ptr<MaterialManager>;

	MaterialManager();
	
  static MaterialManager& get();
  
	// Iterates through the list of loaded materials searching for a material with the same name as "name".
	// Returns the first material found with a matching name. Returns an empty pointer if no matching material
	// is found.
	const Material::ShPtr get_material(const std::string name) const;

  // Loads a material from the given path if it hasn't been loaded yet.
  // Returns the existing material if already loaded.
  Material::ShPtr load_material(const std::string& path);

  // Loads all materials from an MTL file, storing each under its newmtl name.
  void load_mtl_library(const std::string& path);

 private:
	bool loaded_;
	std::vector<std::string> mat_names_;
	std::vector<Material::ShPtr> mats_;
	
	void init();
  
  // Iterates through the list of materials that need to be loaded and loads them. First checks to see if
	// materials have been loaded to prevent duplicate loadings
	void load_materials();
  
  // OBJ Decoding stuff -------------
  Material::ShPtr decode(FileBlob& b);
  // Returns the index of the first character following a group of newline characters after the offset
  unsigned int newline_index(const FileBlob& b, unsigned int offset) const;
  // Returns a collection of whitespace-separated character strings occuring between offset and the end of the
  // line
  const std::vector<std::string> Tokenize(const FileBlob& b, const unsigned int offset) const;
	
	MaterialManager(const MaterialManager&) = delete;
	MaterialManager& operator=(const MaterialManager&) = delete;
};

#endif
