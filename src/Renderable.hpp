#ifndef COSMOS_RENDERABLE_H_
#define COSMOS_RENDERABLE_H_

#include <boost/shared_ptr.hpp>
//#include <vector>
#include <string>

//#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Material.hpp"
#include "Mesh.hpp"
#include "util.hpp"
//#include "Vector2f.hpp"
//#include "Vector3f.hpp"

/*
 * A class for representing the renderable parts of objects in the game
 */
class Renderable {
 public:
	typedef boost::shared_ptr<Renderable> ShPtr;

	Renderable(unsigned int id): id_(id) {}
  const unsigned int id() const { return id_; }
  
  Renderable& set_material(Material::ShPtr mat) { material_ = mat; textured_ = mat->is_textured(); return *this; }
  Renderable& set_mesh(Mesh::ShPtr mesh) { mesh_ = mesh; return *this; }
  
  void render() const;

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
