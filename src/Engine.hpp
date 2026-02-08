#ifndef COSMOS_ENGINE_HPP_
#define COSMOS_ENGINE_HPP_

#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>

#include <glm/glm.hpp>

#include <vector>

#include "Camera.hpp"
#include "DebugAxes.hpp"
#include "InputManager.hpp"
#include "TileGrid.hpp"
#include "ParticleEmitter.hpp"
#include "ParticleEmitterDef.hpp"
#include "ShadowCamera.hpp"
#include "SkyBox.hpp"
#include "TerrainData.hpp"
#include "Timer.hpp"
#include "util.hpp"

class GameScript;

class Engine {
public:
    Engine(int width, int height, const char* title);
    ~Engine();

    void run(GameScript& game);

    int screen_width() const { return screen_width_; }
    int screen_height() const { return screen_height_; }
    InputManager& input_manager() { return input_manager_; }
    SDL_Window* window() { return window_; }

    Camera& camera() { return camera_; }
    ShadowCamera& shadow_camera() { return shadow_camera_; }
    void set_tile_grid(const TileGrid& tg) { tile_grid_ = tg; }
    void set_terrain(TerrainData::ShPtr t) { terrain_ = t; }
    void set_skybox(SkyBox::ShPtr s) { skybox_ = s; }
    void add_particle_emitter(ParticleEmitter::ShPtr e) { emitters_.push_back(e); }
    void set_particle_emitter(ParticleEmitter::ShPtr e) {
        emitters_.clear();
        if (e) emitters_.push_back(e);
    }
    void set_light_direction(const glm::vec3& d) { light_dir_ = d; }
    void set_light_ambient(const glm::vec4& a) { light_ambient_ = a; }
    void set_light_diffuse(const glm::vec4& d) { light_diffuse_ = d; }
    void set_light_specular(const glm::vec4& s) { light_specular_ = s; }
    void set_ambient_global(const glm::vec4& a) { ambient_global_ = a; }

private:
    int screen_width_;
    int screen_height_;
    SDL_Window* window_;
    SDL_GLContext gl_context_;
    Timer timer_;
    InputManager input_manager_;

    Camera camera_;
    ShadowCamera shadow_camera_;
    GLuint shadow_buffer_;
    GLuint hdr_frame_buffer_;
    GLuint hdr_depth_rb_;

    // HDR display state
    bool hdr_enabled_;
    float hdr_headroom_;
    float sdr_white_level_;

    TileGrid tile_grid_;
    TerrainData::ShPtr terrain_;
    SkyBox::ShPtr skybox_;
    Mesh::ShPtr hud_quad_;
    std::vector<ParticleEmitter::ShPtr> emitters_;

    glm::vec3 light_dir_;
    glm::vec4 light_ambient_;
    glm::vec4 light_diffuse_;
    glm::vec4 light_specular_;
    glm::vec4 ambient_global_;

    DebugAxes debug_axes_;

    void init_fbos();
    void render();
    void update_engine(float dt);

    DISALLOW_COPY_AND_ASSIGN(Engine);
};

#endif
