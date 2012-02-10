#ifndef COSMOS_RENDERABLE_H_
#define COSMOS_RENDERABLE_H_

#include <boost/shared_ptr.hpp>
#include <string>

//#include "SDL/SDL_opengl.h"
#define GL_GLEXT_PROTOTYPES
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glext.h"

#include "ResourceManager/MaterialManager.hpp"
#include "ResourceManager/MeshManager.hpp"

#include "Message.hpp"
#include "Transform.hpp"
#include "util.hpp"

/*
 * A class for representing the renderable parts of objects in the game
 */
class Renderable {
 public:
  typedef boost::shared_ptr<Renderable> ShPtr;

  Renderable(unsigned int id, Transform::ShPtr transform, MaterialManager::ShPtr mat_man, MeshManager::ShPtr mesh_man)
   : id_(id), transform_(transform), mat_man_(mat_man), mesh_man_(mesh_man) {}
  const unsigned int id() const { return id_; }

  void handle_message(Message::ShPtr msg);

  Renderable& set_material(Material::ShPtr mat) { material_ = mat; textured_ = mat->get_texture(); return *this; }
  Renderable& set_mesh(Mesh::ShPtr mesh) { mesh_ = mesh; return *this; }

  void render() const;
  void draw_geometry() const;

 protected:
  //Single mesh per object for now
  Material::ShPtr material_;
  Mesh::ShPtr mesh_;
  Transform::ShPtr transform_;

 private:
  bool textured_;
  const unsigned int id_;
  
  MaterialManager::ShPtr mat_man_;
  MeshManager::ShPtr mesh_man_;

  DISALLOW_COPY_AND_ASSIGN(Renderable);
};

#endif
