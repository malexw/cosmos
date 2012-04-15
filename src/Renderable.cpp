#include <string>
#include <vector>

#include <boost/pointer_cast.hpp>

#include "CosmosConfig.hpp"

#include "ResourceManager/ShaderManager.hpp"
#include "ResourceManager/TextureManager.hpp"

#include "Renderable.hpp"
#include "Vector2f.hpp"

void Renderable::handle_message(Message::ShPtr msg) {

  if (msg->type_ == Message::RENDERABLE_SET) {
    if (msg->has_key("material")) {
      set_material(mat_man_->get_material(msg->get_string("material")));
    }
    if (msg->has_key("mesh")) {
      set_mesh(mesh_man_->get_mesh(msg->get_string("mesh")));
    }
  }
}

void Renderable::render() const {
  if (!CosmosConfig::get().is_textures()) {
    draw_geometry();
    return;
  }

  material_->apply();
  mesh_->draw();
  material_->tidy();
  //if (material_->is_bump_mapped() && CosmosConfig::get().is_bump_mapping()) {
  if (false) {
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
  }
}

// For passes without shaders (ie shadow mapping)
void Renderable::draw_geometry() const {
  if (textured_) {
    glBindTexture(GL_TEXTURE_2D, material_->get_texture()->get_index());
  }
  mesh_->draw();
}
