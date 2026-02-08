#ifndef COSMOS_MATERIAL_H_
#define COSMOS_MATERIAL_H_

#include <string>

#include <memory>

#include "Texture.hpp"
#include "Shaders/ShaderProgram.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

class Material {
public:
	typedef std::shared_ptr<Material> ShPtr;

	Material(std::string name);
	
	// Returns the name of the Material
	std::string get_name() const;

	// Compare the name of this Material with another name. Returns true if they're equal, false otherwise.
	bool is_name(const std::string& rhs) const;

	Material& set_texture(Texture::ShPtr tex) { diff_tex_ = tex; textured_ = true; return *this; }
  const Texture::ShPtr get_texture() const { return diff_tex_; }
  
  Material& set_bump_tex(Texture::ShPtr bt) { bump_tex_ = bt; return *this; }
  const Texture::ShPtr get_bump_tex() const { return bump_tex_; }
  
  Material& set_n11n_tex(Texture::ShPtr nt) { n11n_tex_ = nt; return *this; }
  const Texture::ShPtr get_n11n_tex() const { return n11n_tex_; }
  
  Material& set_decal_tex(Texture::ShPtr dt) { decal_tex_ = dt; return *this; }
  const Texture::ShPtr get_decal_tex() const { return decal_tex_; }

  Material& set_shader(ShaderProgram::ShPtr s) { shader_ = s; return *this; }
  const ShaderProgram::ShPtr shader() const { return shader_; }

  Material& set_ambient_color(Vector3f color) { ambient_color_ = color; return *this; }
  Vector3f get_ambient_color() const { return ambient_color_; }

  Material& set_diffuse_color(Vector3f color) { diffuse_color_ = color; return *this; }
  Vector3f get_diffuse_color() const { return diffuse_color_; }
  
  Material& set_specular_color(Vector3f color) { specular_color_ = color; return *this; }
  Vector3f get_specular_color() const { return specular_color_; }

  bool is_textured() const { return textured_; }
  bool is_bump_mapped() const { return bump_tex_ != nullptr; }
  bool is_decal_mapped() const { return decal_tex_ != nullptr; }

  void bind() const;

private:
  bool textured_;
	std::string name_;
  Vector3f ambient_color_;
  Vector3f diffuse_color_;
  Vector3f specular_color_;
  Texture::ShPtr diff_tex_;
  Texture::ShPtr bump_tex_;
  Texture::ShPtr n11n_tex_;
  Texture::ShPtr decal_tex_;
  ShaderProgram::ShPtr shader_;

	DISALLOW_COPY_AND_ASSIGN(Material);
};

#endif
