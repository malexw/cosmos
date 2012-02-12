#include "Material.hpp"

#include "Renderer.hpp"

void Material::apply() const {
  if (diff_tex_) {
    // TODO Figure out which one I need to be using
    glActiveTexture(GL_TEXTURE0);
    glClientActiveTexture(GL_TEXTURE0);
    //
    
    glBindTexture(GL_TEXTURE_2D, diff_tex_->get_index());
  }
  if (bump_tex_) {
    // TODO See above
    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    //
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, bump_tex_->get_index());
  }
  
  //shader_program_->run();
}

void Material::tidy() const {
  //glUseProgram(0);
  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);
  glClientActiveTexture(GL_TEXTURE0);
  glActiveTexture(GL_TEXTURE0);
}
