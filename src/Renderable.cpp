#include "Renderable.hpp"

void Renderable::render() const {
  if (textured_) {
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
  glDrawArrays(GL_TRIANGLES, 0, mesh_->triangle_count() * 3);
}
