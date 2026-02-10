#ifndef COSMOS_PARTICLEEMITTER_H_
#define COSMOS_PARTICLEEMITTER_H_

#include <memory>
#include <vector>
#include <math.h>

#include <SDL3/SDL_opengl.h>
#include <glm/glm.hpp>

#include "Mesh.hpp"
#include "Particle.hpp"
#include "ParticleEmitterDef.hpp"
#include "Quaternion.hpp"
#include "Texture.hpp"
#include "Transform.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

/*
 * An emitter of fancy hats
 */
class ParticleEmitter {
 public:
	using ShPtr = std::shared_ptr<ParticleEmitter>;

	ParticleEmitter(ParticleEmitterDef::ShPtr def, Vector3f position, Vector3f direction, Vector3f up);
  ~ParticleEmitter();

  void update(float delta);
  void render(Transform::ShPtr cam, const glm::mat4& projView);
  void rotate(const Vector3f& axis, float angle);
  void set_position(const Vector3f& pos) { pos_ = pos; }
  ParticleBlendMode blend_mode() const { return def_->blend_mode; }

 private:
  struct ParticleInstance {
      glm::vec3 position;  // 12 bytes
      glm::vec4 color;     // 16 bytes
      float scale;          // 4 bytes
  };                        // 32 bytes total

  ParticleEmitterDef::ShPtr def_;
  Mesh::ShPtr mesh_;
  Texture::ShPtr texture_;

  Vector3f pos_;
  Vector3f norm_;
  Vector3f up_;
  std::vector<Particle::ShPtr> parts_;
  int part_count_;
  float generation_rate_;
  float generation_remainder_;
  int next_part_;

  GLuint particle_vao_;
  GLuint instance_vbo_;
  int max_particles_;

  void init();

  ParticleEmitter(const ParticleEmitter&) = delete;
  ParticleEmitter& operator=(const ParticleEmitter&) = delete;
};

#endif
