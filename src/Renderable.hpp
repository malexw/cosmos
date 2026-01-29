#ifndef COSMOS_RENDERABLE_H_
#define COSMOS_RENDERABLE_H_

#include <memory>
#include <string>

#include "Material.hpp"
#include "Mesh.hpp"
#include "util.hpp"

/*
 * A class for representing the renderable parts of objects in the game
 */
class Renderable {
 public:
	typedef std::shared_ptr<Renderable> ShPtr;

	Renderable(unsigned int id): id_(id) {}
  unsigned int id() const { return id_; }
  
  Renderable& set_material(Material::ShPtr mat) { material_ = mat; textured_ = mat->is_textured(); return *this; }
  Renderable& set_mesh(Mesh::ShPtr mesh) { mesh_ = mesh; return *this; }
  
  void render() const;
  void draw_geometry() const;

 protected:
  //Single mesh per object for now
  Material::ShPtr material_;
  Mesh::ShPtr mesh_;

 private:
  bool textured_;
  const unsigned int id_;
  
  DISALLOW_COPY_AND_ASSIGN(Renderable);
};

#endif
