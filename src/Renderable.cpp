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
  //if (material_->is_bump_mapped() && CosmosConfig::get().is_bump_mapping()) {
  if (false) {
    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, material_->get_bump_tex()->get_index());
    
    /// Decals disabled until I can figure out how to write a proper decal system
    /*
    // assume true for now
    //if (material_->has_decals()) {
    if (CosmosConfig::get().is_decals()) {
      glActiveTexture(GL_TEXTURE2);
      //glEnable(GL_TEXTURE_2D); // probably never necessary
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY); // no need for this for now
      glBindTexture(GL_TEXTURE_2D, TextureManager::get().get_texture("res/textures/decal-test.png")->get_index());
      ShaderManager::get().get_shader_program("bumpdec")->run();
    } else {
      ShaderManager::get().get_shader_program("bump")->run();
    } */
    //std::cout << "Tex " << material_->get_texture()->get_index() << " Bump " << material_->get_bump_tex()->get_index() << std::endl;
    
    mesh_->set_tex_pointer();
    
    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
    
    glUseProgram(0);
    //glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
}

// For passes without shaders (ie shadow mapping)
void Renderable::draw_geometry() const {
  if (textured_) {
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
  glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}
