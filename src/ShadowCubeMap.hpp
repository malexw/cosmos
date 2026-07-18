#ifndef COSMOS_SHADOWCUBEMAP_HPP_
#define COSMOS_SHADOWCUBEMAP_HPP_

#include <functional>

#include <glad/gl.h>
#include <glm/glm.hpp>

// Omnidirectional shadow map for a point light: a depth cube texture rendered
// as six 90-degree faces from the light's position. Depth-only; sampling in
// the lit shaders is wired up separately.
class ShadowCubeMap {
public:
    ShadowCubeMap();
    ~ShadowCubeMap();

    void init(int map_size);

    // Recomputes the six face matrices for a light at `position` whose
    // influence ends at `radius` (used as the far plane).
    void update(const glm::vec3& position, float radius);

    // Renders all six faces. The callback receives the face's combined
    // projection * view matrix and the face index (0-5, +X -X +Y -Y +Z -Z).
    void render(const std::function<void(const glm::mat4& proj_view, int face)>& draw_callback);

    void bind_texture(GLenum unit) const;

    const glm::vec3& position() const { return position_; }
    float near_plane() const { return near_; }
    float far_plane() const { return far_; }

private:
    glm::vec3 position_;
    float near_;
    float far_;
    glm::mat4 face_matrices_[6];
    int map_size_;
    GLuint fbo_;
    GLuint depth_cube_tex_;

    ShadowCubeMap(const ShadowCubeMap&) = delete;
    ShadowCubeMap& operator=(const ShadowCubeMap&) = delete;
};

#endif
