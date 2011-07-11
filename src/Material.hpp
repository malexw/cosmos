#ifndef COSMOS_MATERIAL_H_
#define COSMOS_MATERIAL_H_

#include <string>

#include <boost/shared_ptr.hpp>

#include "Texture.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

class Material {
public:
	typedef boost::shared_ptr<Material> ShPtr;

	Material(std::string name);
	
	// Returns the name of the Material
	const std::string get_name() const;
	
	// Compare the name of this Material with another name. Returns true if they're equal, false otherwise.
	const bool is_name(const std::string& rhs) const;

	Material& set_texture(Texture::ShPtr tex) { diff_tex_ = tex; textured_ = true; return *this; }
  const Texture::ShPtr get_texture() const { return diff_tex_; }
  
  Material& set_bump_tex(Texture::ShPtr bt) { bump_tex_ = bt; return *this; }
  const Texture::ShPtr get_bump_tex() const { return bump_tex_; }
  
  Material& set_diff_color(Vector3f color) { diff_color_ = color; return *this; }
  Vector3f get_diff_color() const { return diff_color_; }
  
  const bool is_textured() const { return textured_; }

private:
  bool textured_;
	std::string name_;
  Vector3f diff_color_;
  Texture::ShPtr diff_tex_;
  Texture::ShPtr bump_tex_;

	DISALLOW_COPY_AND_ASSIGN(Material);
};

#endif
