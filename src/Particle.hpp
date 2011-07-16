#ifndef COSMOS_PARTICLE_H_
#define COSMOS_PARTICLE_H_

#include <boost/shared_ptr.hpp>
#include <iostream>
//#include <vector>
//#include <string>

//#include "SDL/SDL.h"
#include "SDL/SDL_opengl.h"

#include "Camera.hpp"
#include "Matrix4f.hpp"
#include "Renderable.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * A fancy hat
 */
class Particle {
 public:
	typedef boost::shared_ptr<Particle> ShPtr;

  // constant speed particles for now
	Particle(Renderable::ShPtr renderable)
   : renderable_(renderable), lifetime_(0.0f) {}

  void update(float delta);
  void render(Camera::ShPtr cam);

  void reset(Vector3f position, Vector3f velocity, float lifetime) {
    pos_ = position;
    velo_ = velocity;
    lifetime_ = lifetime;
  }

 private:
  Renderable::ShPtr renderable_; // Shouldn't use this - designed for GameObjects
  Vector3f pos_; // position in world coordinates
  Vector3f velo_;
  float lifetime_; // lifetime in seconds
  
  DISALLOW_COPY_AND_ASSIGN(Particle);
};

#endif
