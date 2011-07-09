#ifndef COSMOS_MATERIAL_H_
#define COSMOS_MATERIAL_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "Texture.hpp"
#include "util.hpp"

class Material {
public:
	typedef boost::shared_ptr<Material> ShPtr;

	Material(std::string name);
	
	// Returns the name of the Material
	const std::string get_name() const;
	
	// Compare the name of this Material with another name. Returns true if they're equal, false otherwise.
	const bool is_name(const std::string& rhs) const;

	Material& set_texture(Texture::ShPtr tex) { texture_ = tex; textured_ = true; return *this; }
  const Texture::ShPtr get_texture() const { return texture_; }
  
  const bool is_textured() const { return textured_; }

private:
  bool textured_;
	std::string name_;
  Texture::ShPtr texture_;

	DISALLOW_COPY_AND_ASSIGN(Material);
};

#endif
