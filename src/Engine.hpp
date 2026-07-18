#ifndef COSMOS_ENGINE_HPP_
#define COSMOS_ENGINE_HPP_

#include <SDL3/SDL.h>
#include <glad/gl.h>

#include <imgui.h>
#include <imgui_impl_opengl3.h>

#include <glm/glm.hpp>

#include <memory>
#include <vector>

#include "Camera.hpp"
#include "DebugAxes.hpp"
#include "DisplayConfig.hpp"
#include "InputManager.hpp"
#include "Light.hpp"
#include "RenderTarget.hpp"
#include "TileGrid.hpp"
#include "ParticleEmitter.hpp"
#include "ParticleEmitterDef.hpp"
#include "ShadowCubeMap.hpp"
#include "ShadowMapManager.hpp"
#include "SkyBox.hpp"
#include "TerrainData.hpp"
#include "Timer.hpp"
#include "util.hpp"

class GameScript;

class Engine {
public:
    Engine(const DisplayConfig& display, const char* title);
    ~Engine();

    void run(GameScript& game);

    int screen_width() const { return screen_width_; }
    int screen_height() const { return screen_height_; }
    bool float_framebuffer() const { return float_framebuffer_; }
    InputManager& input_manager() { return input_manager_; }
    SDL_Window* window() { return window_; }

    Camera& camera() { return camera_; }
    void set_tile_grid(const TileGrid& tg) { tile_grid_ = tg; }
    void set_terrain(TerrainData::ShPtr t) { terrain_ = t; }
    void set_skybox(SkyBox::ShPtr s) { skybox_ = s; }
    void add_particle_emitter(ParticleEmitter::ShPtr e) { emitters_.push_back(e); }
    void set_particle_emitter(ParticleEmitter::ShPtr e) {
        emitters_.clear();
        if (e) emitters_.push_back(e);
    }
    // Stores the scene's lights, moving the shadow-casting directional light
    // (if any) to the front so it lands at index 0 in the PerFrame UBO.
    void set_lights(const std::vector<Light>& lights);
    const std::vector<Light>& lights() const { return lights_; }
    // The shadow-casting directional light, or nullptr if the scene has none.
    const Light* sun() const;

private:
    int screen_width_;
    int screen_height_;
    SDL_Window* window_;
    SDL_GLContext gl_context_;
    Timer timer_;
    InputManager input_manager_;

    Camera camera_;
    ShadowMapManager shadow_manager_;
    ShadowCubeMap point_shadow_;
    bool point_shadow_rendered_ = false;
    std::unique_ptr<RenderTarget> hdr_target_;
    std::unique_ptr<RenderTarget> ssao_target_;
    std::unique_ptr<RenderTarget> ssao_blur_target_;
    GLuint ssao_noise_tex_ = 0;
    std::vector<glm::vec3> ssao_kernel_;

    // HDR display state
    bool float_framebuffer_;
    bool hdr_enabled_;
    float hdr_headroom_;
    float sdr_white_level_;

    TileGrid tile_grid_;
    TerrainData::ShPtr terrain_;
    SkyBox::ShPtr skybox_;
    Mesh::ShPtr hud_quad_;
    std::vector<ParticleEmitter::ShPtr> emitters_;

    std::vector<Light> lights_;

    DebugAxes debug_axes_;

    void init_fbos();
    void render();
    void render_shadow_pass(const ShadowMap& cascade, int index);
    void render_depth_geometry(const glm::mat4& proj_view);
    void render_hdr_pass();
    void render_ssao_pass();
    void render_ssao_blur_pass();
    void render_resolve_pass();
    void update_engine(float dt);

    Engine(const Engine&) = delete;
    Engine& operator=(const Engine&) = delete;
};

#endif
