#ifndef COSMOS_RENDERABLE_H_
#define COSMOS_RENDERABLE_H_

#include <memory>
#include <string>

#include "Mesh.hpp"
#include "util.hpp"

/*
 * A class for representing the renderable parts of objects in the game
 */
class Renderable {
 public:
	using ShPtr = std::shared_ptr<Renderable>;

	Renderable(unsigned int id): id_(id) {}
  unsigned int id() const { return id_; }
  
  Renderable& set_mesh(Mesh::ShPtr mesh) { mesh_ = mesh; return *this; }
  
  void render() const;
  void draw_geometry() const;

 protected:
  //Single mesh per object for now
  Mesh::ShPtr mesh_;

 private:
  const unsigned int id_;
  
  Renderable(const Renderable&) = delete;
  Renderable& operator=(const Renderable&) = delete;
};

#endif
