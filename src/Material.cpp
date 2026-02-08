#include <SDL3/SDL_opengl.h>

#include "Material.hpp"
#include "CosmosConfig.hpp"

Material::Material(std::string name)
  : textured_(false), name_(name) { }

std::string Material::get_name() const {
	return name_;
}

bool Material::is_name(const std::string& rhs) const {
	return name_.compare(rhs) == 0;
}

void Material::bind() const {
  shader_->run();

  if (textured_) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diff_tex_->get_index());
  }

  bool use_bump = bump_tex_ != nullptr && CosmosConfig::get().is_bump_mapping();
  bool use_decal = use_bump && decal_tex_ != nullptr && CosmosConfig::get().is_decals();

  GLint loc;
  loc = glGetUniformLocation(shader_->get_id(), "has_bump_map");
  if (loc >= 0) glUniform1i(loc, use_bump ? 1 : 0);

  loc = glGetUniformLocation(shader_->get_id(), "has_decal");
  if (loc >= 0) glUniform1i(loc, use_decal ? 1 : 0);

  if (use_bump) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, bump_tex_->get_index());
  }
  if (use_decal) {
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, decal_tex_->get_index());
  }
}
