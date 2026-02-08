#include <vector>

#include "CosmosConfig.hpp"

#include "ResourceManager/TextureManager.hpp"

#include "Renderable.hpp"
#include "Vector2f.hpp"

void Renderable::render() const {
  if (!CosmosConfig::get().is_textures()) {
    draw_geometry();
    return;
  }

  mesh_->bind();
  int offset = 0;
  for (const Mesh::Submesh& sub : mesh_->submeshes()) {
    int count = sub.triangle_count * 3;
    sub.material->bind();
    glDrawArrays(GL_TRIANGLES, offset, count);
    offset += count;
  }
  glBindVertexArray(0);
}

// For passes without shaders (ie shadow mapping)
void Renderable::draw_geometry() const {
  mesh_->bind();
  int offset = 0;
  for (const Mesh::Submesh& sub : mesh_->submeshes()) {
    Material::ShPtr mat = sub.material;
    bool textured = mat && mat->is_textured();
    if (textured) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, mat->get_texture()->get_index());
    }
    glDrawArrays(GL_TRIANGLES, offset, sub.triangle_count * 3);
    offset += sub.triangle_count * 3;
  }
  glBindVertexArray(0);
}
