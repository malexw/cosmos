#ifndef COSMOS_PARTICLEEMITTERDEF_HPP_
#define COSMOS_PARTICLEEMITTERDEF_HPP_

#include <memory>
#include <string>
#include <glm/glm.hpp>

enum class ParticleBlendMode { Additive, Alpha };

struct ParticleEmitterDef {
    using ShPtr = std::shared_ptr<ParticleEmitterDef>;

    std::string mesh_path;     // .tres path for particle mesh
    std::string texture_path;  // .tres path for particle texture

    float emission_rate;       // particles per second
    float lifetime;            // seconds
    float speed;               // initial speed
    float spread;              // cone half-angle in degrees

    glm::vec4 start_color;    // RGBA at birth
    glm::vec4 end_color;      // RGBA at death
    float start_size;          // scale at birth
    float end_size;            // scale at death

    glm::vec3 gravity;         // acceleration (world space)
    ParticleBlendMode blend_mode;

    ParticleEmitterDef()
        : emission_rate(30.0f), lifetime(2.0f), speed(3.0f), spread(20.0f),
          start_color(1, 1, 1, 1), end_color(1, 1, 1, 0),
          start_size(1.0f), end_size(1.0f),
          gravity(0, 0, 0), blend_mode(ParticleBlendMode::Additive) {}
};

#endif
