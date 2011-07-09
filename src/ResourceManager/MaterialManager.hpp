#ifndef COSMOS_MATERIALMANAGER_H_
#define COSMOS_MATERIALMANAGER_H_

#include <vector>

#include <boost/shared_ptr.hpp>

#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Material.hpp"
#include "util.hpp"

class MaterialManager {
 public:
	typedef boost::shared_ptr<MaterialManager> ShPtr;

	MaterialManager();
	
  static MaterialManager& get();
  
	// Iterates through the list of loaded materials searching for a material with the same name as "name".
	// Returns the first material found with a matching name. Returns an empty pointer if no matching material
	// is found.
	const Material::ShPtr get_material(const std::string name) const;

 private:
	bool loaded_;
	std::vector<std::string> mat_names_;
	std::vector<Material::ShPtr> mats_;
	
	void init();
  
  // Iterates through the list of materials that need to be loaded and loads them. First checks to see if
	// materials have been loaded to prevent duplicate loadings
	void load_materials();
	
	DISALLOW_COPY_AND_ASSIGN(MaterialManager);
};

#endif
