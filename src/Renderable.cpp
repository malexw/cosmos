#include <vector>

#include "ResourceManager/ShaderManager.hpp"

#include "Renderable.hpp"
#include "Vector2f.hpp"

void Renderable::render() const {
  if (textured_) {
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
  if (material_->is_bump_mapped()) {
    //glBindTexture(GL_TEXTURE_2D, material_->get_bump_tex()->get_index());
    glActiveTexture(GL_TEXTURE1);
    glClientActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindTexture(GL_TEXTURE_2D, material_->get_bump_tex()->get_index());
    
    //std::cout << "Tex " << material_->get_texture()->get_index() << " Bump " << material_->get_bump_tex()->get_index() << std::endl;
    
    ShaderProgram::ShPtr prog = ShaderManager::get().get_shader_program("bump");
    prog->run();
    
    /*glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB,  GL_DOT3_RGB);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
    glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
    glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);*/
    /*glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
    glTexEnvi(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC0_RGB, GL_TEXTURE);
    glTexEnvi(GL_TEXTURE_ENV, GL_SRC1_RGB, GL_PREVIOUS);*/
    mesh_->set_tex_pointer();
    
    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
    
    glUseProgram(0);
    //glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glClientActiveTexture(GL_TEXTURE0);
    glActiveTexture(GL_TEXTURE0);
  } else {
    glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
  }
}
