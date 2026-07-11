#ifndef COSMOS_LIGHT_HPP_
#define COSMOS_LIGHT_HPP_

#include <glm/glm.hpp>

// Maximum lights uploaded to the PerFrame UBO; must match the array sizes
// in the shader PerFrame blocks.
constexpr int kMaxLights = 8;

struct Light {
    enum class Type : uint8_t { Directional, Point, Spot };

    Type type = Type::Directional;

    glm::vec3 direction{0.0f, -1.0f, 0.0f};  // directional/spot
    glm::vec3 position{0.0f};                  // point/spot
    glm::vec3 color{1.0f};                     // RGB
    float intensity = 1.0f;

    // Attenuation cutoff distance (point/spot)
    float radius = 10.0f;

    // Spot cone (unused for now)
    float inner_cone = 0.0f;  // radians
    float outer_cone = 0.0f;  // radians

    // Shadow config
    bool cast_shadows = false;
};

#endif
