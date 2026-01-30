#include <vector>

#include "CosmosConfig.hpp"

#include "ResourceManager/ShaderManager.hpp"
#include "ResourceManager/TextureManager.hpp"

#include "Renderable.hpp"
#include "Vector2f.hpp"

void Renderable::render() const {
  if (!CosmosConfig::get().is_textures()) {
    draw_geometry();
    return;
  }

  if (textured_) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
  if (material_->is_bump_mapped() && CosmosConfig::get().is_bump_mapping()) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, material_->get_bump_tex()->get_index());

    if (CosmosConfig::get().is_decals()) {
      glActiveTexture(GL_TEXTURE2);
      glBindTexture(GL_TEXTURE_2D, TextureManager::get().get_texture("res/textures/decal-test.png")->get_index());
      ShaderManager::get().get_shader_program("bumpdec")->run();
    } else {
      ShaderManager::get().get_shader_program("bump")->run();
    }

    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);

    glUseProgram(0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
    glBindVertexArray(0);
  }
}

// For passes without shaders (ie shadow mapping)
void Renderable::draw_geometry() const {
  if (textured_) {
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
  glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
  glBindVertexArray(0);
}
