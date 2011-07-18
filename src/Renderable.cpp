#include <vector>

#include "ResourceManager/ShaderManager.hpp"
#include "ResourceManager/TextureManager.hpp"

#include "Renderable.hpp"
#include "Vector2f.hpp"

void Renderable::render() const {
  if (textured_) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
  if (material_->is_bump_mapped()) {
    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, material_->get_bump_tex()->get_index());
    
    // assume true for now
    //if (material_->has_decals()) {
      glActiveTexture(GL_TEXTURE2);
      //glEnable(GL_TEXTURE_2D); // probably never necessary
      //glEnableClientState(GL_TEXTURE_COORD_ARRAY); // no need for this for now
      glBindTexture(GL_TEXTURE_2D, TextureManager::get().get_texture("res/textures/decal-test.png")->get_index());
    //}
    
    //std::cout << "Tex " << material_->get_texture()->get_index() << " Bump " << material_->get_bump_tex()->get_index() << std::endl;
    
    ShaderProgram::ShPtr prog = ShaderManager::get().get_shader_program("bump");
    prog->run();
    
    mesh_->set_tex_pointer();
    
    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
    
    glUseProgram(0);
    //glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
  }
}

// For passes without shaders (ie shadow mapping)
void Renderable::draw_geometry() const {
  if (textured_) {
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
  glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
}
