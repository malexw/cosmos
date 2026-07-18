#include <algorithm>
#include <cfloat>
#include <iostream>

#include <random>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <imgui_impl_sdl3.h>

#include "Engine.hpp"
#include "CosmosConfig.hpp"
#include "GameObjectManager.hpp"
#include "GameScript.hpp"
#include "RenderPass.hpp"
#include "ResourceManager/AudioManager.hpp"
#include "ResourceManager/MeshManager.hpp"
#include "ResourceManager/ResourceManager.hpp"
#include "ResourceManager/ShaderManager.hpp"
#include "ResourceManager/TextureManager.hpp"
#include "Vector3f.hpp"

Engine::Engine(const DisplayConfig& display, const char* title)
    : screen_width_(0),
      screen_height_(0),
      window_(nullptr),
      gl_context_(nullptr),
      float_framebuffer_(false),
      hdr_enabled_(false),
      hdr_headroom_(1.0f),
      sdr_white_level_(1.0f) {

    srand(31337);

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    // Determine window dimensions
    int win_w = display.width;
    int win_h = display.height;
    Uint64 flags = SDL_WINDOW_OPENGL;
    if (display.windowed_fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
        const SDL_DisplayMode* mode = SDL_GetDesktopDisplayMode(SDL_GetPrimaryDisplay());
        if (mode) {
            win_w = mode->w;
            win_h = mode->h;
        }
    }

    // Try float framebuffer for HDR output, fall back to integer
    SDL_GL_SetAttribute(SDL_GL_FLOATBUFFERS, 1);
    window_ = SDL_CreateWindow(title, win_w, win_h, flags);
    if (window_ != nullptr) {
        float_framebuffer_ = true;
    } else {
        std::cout << "Float framebuffer not available, falling back to integer" << std::endl;
        SDL_GL_SetAttribute(SDL_GL_FLOATBUFFERS, 0);
        window_ = SDL_CreateWindow(title, win_w, win_h, flags);
    }
    if (window_ == nullptr) {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return;
    }

    gl_context_ = SDL_GL_CreateContext(window_);
    if (gl_context_ == nullptr) {
        std::cout << "SDL_GL_CreateContext failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return;
    }

    int gl_version = gladLoadGL(reinterpret_cast<GLADloadfunc>(SDL_GL_GetProcAddress));
    if (gl_version == 0) {
        std::cout << "gladLoadGL failed to load OpenGL functions" << std::endl;
        SDL_GL_DestroyContext(gl_context_);
        gl_context_ = nullptr;
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return;
    }
    std::cout << "OpenGL " << GLAD_VERSION_MAJOR(gl_version) << "."
              << GLAD_VERSION_MINOR(gl_version) << " context" << std::endl;

    // Query pixel size after context creation — on Wayland the compositor
    // configures the fullscreen window size asynchronously, and the EGL
    // surface creation above forces that round-trip to complete.
    SDL_GetWindowSizeInPixels(window_, &screen_width_, &screen_height_);

    SDL_RaiseWindow(window_);

    // Enable adaptive VSync, fall back to standard VSync
    if (!SDL_GL_SetSwapInterval(-1)) {
        SDL_GL_SetSwapInterval(1);
    }

    glViewport(0, 0, screen_width_, screen_height_);

    glEnable(GL_CULL_FACE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(2.0f);

    if (glGetError() != GL_NO_ERROR) {
        //error
    }

    ResourceManager::get();
    ResourceManager::get().load_resource("res/meshes/hdrbox.obj");
    ResourceManager::get().load_resource("res/materials/hdrbox.mtl");

    hud_quad_ = Mesh::create_quad();

    debug_axes_.init();

    std::cout << "Resources loaded" << std::endl;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL3_InitForOpenGL(window_, gl_context_);
    ImGui_ImplOpenGL3_Init("#version 410");
}

Engine::~Engine() {
    // RenderTarget destructors handle FBO + texture cleanup
    hdr_target_.reset();
    ssao_target_.reset();
    ssao_blur_target_.reset();

    if (ssao_noise_tex_) {
        glDeleteTextures(1, &ssao_noise_tex_);
    }
    if (gl_context_) {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplSDL3_Shutdown();
        ImGui::DestroyContext();
    }
    if (gl_context_) {
        SDL_GL_DestroyContext(gl_context_);
    }
    if (window_) {
        SDL_DestroyWindow(window_);
    }
    SDL_Quit();
}

void Engine::init_fbos() {
    int w = screen_width_;
    int h = screen_height_;

    shadow_manager_.init(2048);
    point_shadow_.init(1024);

    hdr_target_ = std::make_unique<RenderTarget>(w, h,
        std::initializer_list<RenderTargetAttachment>{
            {GL_COLOR_ATTACHMENT0, GL_RGBA16F, GL_LINEAR, GL_CLAMP_TO_EDGE},
            {GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT24, GL_NEAREST, GL_CLAMP_TO_EDGE}
        });

    ssao_target_ = std::make_unique<RenderTarget>(w, h,
        std::initializer_list<RenderTargetAttachment>{
            {GL_COLOR_ATTACHMENT0, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE}
        });

    ssao_blur_target_ = std::make_unique<RenderTarget>(w, h,
        std::initializer_list<RenderTargetAttachment>{
            {GL_COLOR_ATTACHMENT0, GL_R8, GL_NEAREST, GL_CLAMP_TO_EDGE}
        });

    // SSAO noise texture (4x4 random tangent-space rotations)
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
    std::vector<float> noise_data(16 * 4); // 16 pixels, RGBA
    for (int i = 0; i < 16; ++i) {
        noise_data[i * 4 + 0] = dist(rng);
        noise_data[i * 4 + 1] = dist(rng);
        noise_data[i * 4 + 2] = 0.0f;
        noise_data[i * 4 + 3] = 0.0f;
    }
    glGenTextures(1, &ssao_noise_tex_);
    glBindTexture(GL_TEXTURE_2D, ssao_noise_tex_);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGBA, GL_FLOAT, noise_data.data());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // SSAO sample kernel (64 hemisphere samples)
    std::uniform_real_distribution<float> dist01(0.0f, 1.0f);
    ssao_kernel_.resize(64);
    for (int i = 0; i < 64; ++i) {
        glm::vec3 sample(dist(rng), dist(rng), dist01(rng));
        sample = glm::normalize(sample);
        sample *= dist01(rng);
        // Scale samples to cluster near origin
        float scale = static_cast<float>(i) / 64.0f;
        scale = 0.1f + scale * scale * (1.0f - 0.1f);
        sample *= scale;
        ssao_kernel_[i] = sample;
    }
}

void Engine::set_lights(const std::vector<Light>& lights) {
    lights_ = lights;
    auto it = std::find_if(lights_.begin(), lights_.end(), [](const Light& l) {
        return l.type == Light::Type::Directional && l.cast_shadows;
    });
    if (it != lights_.end()) {
        std::rotate(lights_.begin(), it, it + 1);
    }
}

const Light* Engine::sun() const {
    if (!lights_.empty() && lights_.front().type == Light::Type::Directional &&
        lights_.front().cast_shadows) {
        return &lights_.front();
    }
    return nullptr;
}

void Engine::run(GameScript& game) {
    if (!window_ || !gl_context_) return;

    init_fbos();
    game.init(*this);

    while (true) {
        timer_.frame_start();

        input_manager_.update();

        // Begin ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL3_NewFrame();
        ImGui::NewFrame();

        float dt = timer_.frame_delta();
        if (!game.update(dt)) break;

        update_engine(dt);

        // Query HDR display state
        SDL_PropertiesID props = SDL_GetWindowProperties(window_);
        hdr_enabled_ = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN, false);
        hdr_headroom_ = SDL_GetFloatProperty(props, SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT, 1.0f);
        sdr_white_level_ = SDL_GetFloatProperty(props, SDL_PROP_WINDOW_SDR_WHITE_LEVEL_FLOAT, 1.0f);

        render();

        // Render ImGui on top of the resolved framebuffer
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        SDL_GL_SwapWindow(window_);
    }

    game.shutdown();
}

void Engine::render() {
    CosmosConfig& config = CosmosConfig::get();

    // GL state from config
    if (!config.is_textures()) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // Shadow pass
    const Light* sun = this->sun();
    if (sun && config.is_shadows()) {
        float aspect = static_cast<float>(screen_width_) / static_cast<float>(screen_height_);
        glm::mat4 camera_view = camera_.transform()->get_inverse_matrix();
        shadow_manager_.update(sun->direction, camera_view,
            glm::radians(45.0f), aspect, 1.0f,
            config.shadow_cascades(), config.shadow_distance());
        shadow_manager_.render([this](const ShadowMap& sm, int i) {
            render_shadow_pass(sm, i);
        });
    }

    // Point-light shadow pass: depth cube for the first shadow-casting
    // point light. Depth only for now — shaders don't sample it yet.
    point_shadow_rendered_ = false;
    if (config.is_shadows()) {
        for (const Light& light : lights_) {
            if (light.type == Light::Type::Directional || !light.cast_shadows) continue;
            point_shadow_.update(light.position, light.radius);
            point_shadow_.render([this](const glm::mat4& proj_view, int /*face*/) {
                render_depth_geometry(proj_view);
            });
            point_shadow_rendered_ = true;
            break;
        }
    }

    // HDR pass
    {
        RenderPassParams hdr_params;
        hdr_params.target = hdr_target_.get();
        hdr_params.clear_color = true;
        hdr_params.clear_depth = true;
        hdr_params.cull_face = GL_BACK;
        RenderPass::execute(hdr_params, [this]() { render_hdr_pass(); });
    }

    // SSAO pass
    if (config.is_ssao()) {
        RenderPassParams ssao_params;
        ssao_params.target = ssao_target_.get();
        ssao_params.clear_color = true;
        ssao_params.depth_test = false;
        ssao_params.depth_write = false;
        RenderPass::execute(ssao_params, [this]() { render_ssao_pass(); });

        // SSAO blur pass
        RenderPassParams blur_params;
        blur_params.target = ssao_blur_target_.get();
        blur_params.clear_color = true;
        blur_params.depth_test = false;
        blur_params.depth_write = false;
        RenderPass::execute(blur_params, [this]() { render_ssao_blur_pass(); });
    }

    // Resolve pass
    {
        RenderPassParams resolve_params;
        resolve_params.viewport_width = screen_width_;
        resolve_params.viewport_height = screen_height_;
        resolve_params.clear_color = true;
        resolve_params.depth_test = false;
        resolve_params.depth_write = false;
        RenderPass::execute(resolve_params, [this]() { render_resolve_pass(); });
    }

    // Restore depth test for next frame
    glEnable(GL_DEPTH_TEST);
}

void Engine::render_shadow_pass(const ShadowMap& cascade, int /*index*/) {
    render_depth_geometry(cascade.projection() * cascade.view_matrix());
}

// Depth-only draw of all shadow-casting geometry, shared by the cascade and
// point-light (cube map) shadow passes.
void Engine::render_depth_geometry(const glm::mat4& proj_view) {
    auto flatProg = ShaderManager::get().get_shader_program("flat");
    flatProg->run();

    // Draw tile grid
    float shadow_half = tile_grid_.tile_size / 2.0f;
    for (int r = 0; r < static_cast<int>(tile_grid_.rows.size()); ++r) {
        const std::string& grid_row = tile_grid_.rows[r];
        for (int c = 0; c < static_cast<int>(grid_row.size()); ++c) {
            auto it = tile_grid_.tiles.find(grid_row[c]);
            if (it == tile_grid_.tiles.end()) continue;
            const TileGrid::Tile& tile = it->second;

            glm::mat4 model = glm::translate(glm::mat4(1.0f),
                glm::vec3(c * tile_grid_.tile_size, 0.0f,
                           -static_cast<float>(r) * tile_grid_.tile_size));
            if (tile.rotation != 0.0f) {
                model = glm::translate(model, glm::vec3(shadow_half, 0.0f, -shadow_half));
                model = glm::rotate(model, glm::radians(tile.rotation),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, glm::vec3(-shadow_half, 0.0f, shadow_half));
            }

            flatProg->setMat4("mvp", proj_view * model);
            tile.mesh->bind();
            glDrawArrays(GL_TRIANGLES, 0, tile.mesh->vertex_count());
        }
    }
    glBindVertexArray(0);

    // Draw renderable game objects with transforms
    for (const auto& obj : GameObjectManager::get().objects()) {
        if (obj->get_transform() && obj->get_renderable()) {
            flatProg->setMat4("mvp", proj_view * obj->get_transform()->get_matrix());
            obj->get_renderable()->draw_geometry();
        }
    }

    // Draw instanced terrain shadows
    if (terrain_) {
        terrain_->render_shadow(proj_view);
    }
}

void Engine::render_hdr_pass() {
    CosmosConfig& config = CosmosConfig::get();
    int width = screen_width_;
    int height = screen_height_;

    glActiveTexture(GL_TEXTURE0);

    // Skybox
    if (skybox_ && config.is_textures() && config.is_skybox()) {
        glm::mat4 skyProj = glm::perspective(glm::radians(45.0f),
            (static_cast<float>(width) / static_cast<float>(height)), 0.1f, 3.0f);
        glm::mat4 skyView = Camera::matrixFromPositionDirection(
            Vector3f(0, 0, 0.8), Vector3f(0, 0, -1));
        glm::mat4 cameraRotInv = camera_.transform()->get_rotation_matrix();
        glm::mat4 skyboxMVP = skyProj * skyView * cameraRotInv;

        glFrontFace(GL_CW);
        auto hdrProg = ShaderManager::get().get_shader_program("hdr");
        hdrProg->run();
        hdrProg->setMat4("mvp", skyboxMVP);
        skybox_->renderable().draw_geometry();
        glFrontFace(GL_CCW);
    }

    // Clear depth so scene draws on top of skybox
    glClear(GL_DEPTH_BUFFER_BIT);

    glm::mat4 mainProj = glm::perspective(glm::radians(45.0f),
        (static_cast<float>(width) / static_cast<float>(height)), 1.0f, 4000.0f);
    glm::mat4 mainView = camera_.transform()->get_inverse_matrix();
    glm::mat4 mainPV = mainProj * mainView;

    // Transform lights into eye space for the PerFrame UBO.
    // lightPosDir: xyz = position (point) or travel direction (directional), w = 0 dir / 1 point
    // lightColor: rgb = color * intensity, w = attenuation radius (point)
    glm::vec4 lightPosDir[kMaxLights];
    glm::vec4 lightColor[kMaxLights];
    int lightCount = 0;
    int pointShadowIndex = -1;
    for (const Light& light : lights_) {
        if (lightCount >= kMaxLights) break;
        if (light.type == Light::Type::Directional) {
            lightPosDir[lightCount] = glm::vec4(glm::mat3(mainView) * light.direction, 0.0f);
            lightColor[lightCount] = glm::vec4(light.color * light.intensity, 0.0f);
        } else {
            // Spot lights are treated as points until cone support lands
            lightPosDir[lightCount] = glm::vec4(
                glm::vec3(mainView * glm::vec4(light.position, 1.0f)), 1.0f);
            lightColor[lightCount] = glm::vec4(light.color * light.intensity, light.radius);
            // First shadow-casting point light matches the cube map rendered this frame
            if (pointShadowIndex < 0 && light.cast_shadows && point_shadow_rendered_) {
                pointShadowIndex = lightCount;
            }
        }
        ++lightCount;
    }

    glm::vec4 pointShadowPos(0.0f);
    glm::vec4 pointShadowParams(0.0f);
    if (pointShadowIndex >= 0) {
        pointShadowPos = glm::vec4(point_shadow_.position(), 0.0f);
        pointShadowParams = glm::vec4(point_shadow_.near_plane(), point_shadow_.far_plane(),
                                      config.shadow_bias(), 0.0f);
        point_shadow_.bind_texture(GL_TEXTURE4);
        glActiveTexture(GL_TEXTURE0);
    }

    // Build shadow matrices array and cascade splits
    glm::mat4 shadowMatrices[4] = {glm::mat4(0.0f), glm::mat4(0.0f), glm::mat4(0.0f), glm::mat4(0.0f)};
    glm::vec4 cascadeSplits(FLT_MAX);
    glm::vec4 cascadeBiases(0.0f);
    int cascadeCount = 0;
    if (sun() && config.is_shadows()) {
        cascadeCount = shadow_manager_.cascade_count();
        for (int i = 0; i < cascadeCount; ++i) {
            shadowMatrices[i] = shadow_manager_.shadow_matrix(i);
        }
        cascadeSplits = shadow_manager_.cascade_splits_eye();
        cascadeBiases = shadow_manager_.cascade_biases(config.shadow_bias());

        shadow_manager_.bind_shadow_texture(GL_TEXTURE3);
        glActiveTexture(GL_TEXTURE0);
    }

    if (config.is_textures()) {
        ShaderManager::get().set_per_frame(mainProj, mainView,
                                            shadowMatrices, cascadeSplits, cascadeCount,
                                            cascadeBiases, lightPosDir, lightColor, lightCount,
                                            pointShadowPos, pointShadowParams, pointShadowIndex);

        // Set shadow debug uniform on lit shaders
        bool shadowDbg = config.is_shadow_debug();
        int pcfMode = config.pcf_mode();
        auto bumpdecProg = ShaderManager::get().get_shader_program("bumpdec");
        bumpdecProg->run();
        bumpdecProg->seti(std::string("debug_shadows"), shadowDbg ? 1 : 0);
        bumpdecProg->seti(std::string("pcf_mode"), pcfMode);

        auto blinnProg = ShaderManager::get().get_shader_program("blinn");
        blinnProg->run();
        blinnProg->seti(std::string("debug_shadows"), shadowDbg ? 1 : 0);
        blinnProg->seti(std::string("pcf_mode"), pcfMode);

        // Draw instanced terrain
        if (terrain_) {
            terrain_->render();
        }
    }

    // Draw tile grid
    float half = tile_grid_.tile_size / 2.0f;
    for (int r = 0; r < static_cast<int>(tile_grid_.rows.size()); ++r) {
        const std::string& grid_row = tile_grid_.rows[r];
        for (int c = 0; c < static_cast<int>(grid_row.size()); ++c) {
            auto it = tile_grid_.tiles.find(grid_row[c]);
            if (it == tile_grid_.tiles.end()) continue;
            const TileGrid::Tile& tile = it->second;

            glm::mat4 model = glm::translate(glm::mat4(1.0f),
                glm::vec3(c * tile_grid_.tile_size, 0.0f,
                           -static_cast<float>(r) * tile_grid_.tile_size));
            if (tile.rotation != 0.0f) {
                model = glm::translate(model, glm::vec3(half, 0.0f, -half));
                model = glm::rotate(model, glm::radians(tile.rotation),
                                    glm::vec3(0.0f, 1.0f, 0.0f));
                model = glm::translate(model, glm::vec3(-half, 0.0f, half));
            }

            if (config.is_textures()) {
                glm::mat3 nm = glm::transpose(glm::inverse(glm::mat3(mainView * model)));
                ShaderManager::get().set_per_draw(model, nm);

                tile.mesh->bind();
                int offset = 0;
                for (const auto& sub : tile.mesh->submeshes()) {
                    sub.material->bind();
                    glDrawArrays(GL_TRIANGLES, offset, sub.triangle_count * 3);
                    offset += sub.triangle_count * 3;
                }
            } else {
                auto flatProg = ShaderManager::get().get_shader_program("flat");
                flatProg->run();
                flatProg->setMat4("mvp", mainPV * model);
                GLint loc = glGetUniformLocation(flatProg->get_id(), "flatColor");
                if (loc >= 0) glUniform4f(loc, 1.0f, 1.0f, 1.0f, 1.0f);

                tile.mesh->bind();
                glDrawArrays(GL_TRIANGLES, 0, tile.mesh->vertex_count());
            }
        }
    }
    glBindVertexArray(0);

    // Render game objects with Transform + Renderable
    for (const auto& obj : GameObjectManager::get().objects()) {
        if (!obj->get_transform() || !obj->get_renderable()) continue;

        glm::mat4 model = obj->get_transform()->get_matrix();

        // Collidable wireframe
        if (obj->get_collidable() && config.is_collidables()) {
            Vector3f pos = obj->get_transform()->get_position();
            glm::mat4 collidable_transform = glm::translate(glm::mat4(1.0f),
                glm::vec3(pos.x(), pos.y(), pos.z()));
            obj->get_collidable()->render_collision(mainProj * mainView * collidable_transform);
        }

        if (config.is_textures()) {
            glm::mat3 cubeNormalMatrix = glm::transpose(glm::inverse(glm::mat3(mainView * model)));
            ShaderManager::get().set_per_draw(model, cubeNormalMatrix);
        } else {
            auto flatProg = ShaderManager::get().get_shader_program("flat");
            flatProg->run();
            flatProg->setMat4("mvp", mainProj * mainView * model);
            GLint loc = glGetUniformLocation(flatProg->get_id(), "flatColor");
            if (loc >= 0) glUniform4f(loc, 1.0f, 1.0f, 1.0f, 1.0f);
        }

        obj->get_renderable()->render();

        // Debug axes at object origin (translation only, no rotation/scale)
        if (config.is_show_axes()) {
            glm::mat4 axesModel = glm::translate(glm::mat4(1.0f), glm::vec3(model[3]));
            debug_axes_.render(axesModel, mainPV, 0.5f);
        }
    }

    // Shadow debug frustum wireframes
    if (config.is_shadow_debug() && sun() && config.is_shadows()) {
        shadow_manager_.render_debug_frustums(mainPV);
    }

    // Particles
    if (config.is_particles() && !emitters_.empty()) {
        glDepthMask(GL_FALSE);
        for (const auto& emitter : emitters_) {
            if (emitter->blend_mode() == ParticleBlendMode::Additive) {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            } else {
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }
            emitter->render(camera_.transform(), mainPV);
        }
        glDepthMask(GL_TRUE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void Engine::render_ssao_pass() {
    int width = screen_width_;
    int height = screen_height_;

    glm::mat4 mainProj = glm::perspective(glm::radians(45.0f),
        (static_cast<float>(width) / static_cast<float>(height)), 1.0f, 4000.0f);

    auto ssaoProg = ShaderManager::get().get_shader_program("ssao");
    ssaoProg->run();
    ssaoProg->setMat4("projection", mainProj);
    GLint samplesLoc = glGetUniformLocation(ssaoProg->get_id(), "samples");
    glUniform3fv(samplesLoc, 64, glm::value_ptr(ssao_kernel_[0]));

    CosmosConfig& config = CosmosConfig::get();
    ssaoProg->setf("radius", config.ssao_radius());
    ssaoProg->setf("bias", config.ssao_bias());
    ssaoProg->setf("power", config.ssao_power());
    GLint screenSizeLoc = glGetUniformLocation(ssaoProg->get_id(), "screenSize");
    glUniform2f(screenSizeLoc, static_cast<float>(width), static_cast<float>(height));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_target_->depth_texture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssao_noise_tex_);

    glm::mat4 ssaoOrtho = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    ssaoProg->setMat4("mvp", ssaoOrtho);
    hud_quad_->bind();
    glDrawArrays(GL_TRIANGLES, 0, hud_quad_->vertex_count());

    glActiveTexture(GL_TEXTURE0);
}

void Engine::render_ssao_blur_pass() {
    auto ssaoBlurProg = ShaderManager::get().get_shader_program("ssao_blur");
    ssaoBlurProg->run();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ssao_target_->color_texture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, hdr_target_->depth_texture());

    glm::mat4 ssaoOrtho = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    ssaoBlurProg->setMat4("mvp", ssaoOrtho);
    hud_quad_->bind();
    glDrawArrays(GL_TRIANGLES, 0, hud_quad_->vertex_count());
}

void Engine::render_resolve_pass() {
    CosmosConfig& config = CosmosConfig::get();

    auto resolveProg = ShaderManager::get().get_shader_program("resolve");
    resolveProg->run();
    resolveProg->seti("hdr_output", hdr_enabled_ ? 1 : 0);
    resolveProg->setf("sdr_white", sdr_white_level_);
    resolveProg->setf("hdr_headroom", hdr_headroom_);
    resolveProg->setf("exposure", config.exposure());
    resolveProg->seti("ao_enabled", config.is_ssao() ? 1 : 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, hdr_target_->color_texture());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ssao_blur_target_->color_texture());
    glm::mat4 resolveProj = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    resolveProg->setMat4("mvp", resolveProj);
    hud_quad_->bind();
    glDrawArrays(GL_TRIANGLES, 0, hud_quad_->vertex_count());

    // Point-light shadow cube debug: draw the selected face as a linearized
    // depth grayscale in the lower-left corner
    if (config.is_shadow_cube_debug() && point_shadow_rendered_) {
        auto cubeDebugProg = ShaderManager::get().get_shader_program("cube_debug");
        cubeDebugProg->run();
        cubeDebugProg->seti("face", config.shadow_cube_face());
        cubeDebugProg->setf("near_plane", point_shadow_.near_plane());
        cubeDebugProg->setf("far_plane", point_shadow_.far_plane());
        cubeDebugProg->setMat4("mvp", resolveProj);
        point_shadow_.bind_texture(GL_TEXTURE0);

        int inset = screen_height_ / 3;
        glViewport(0, 0, inset, inset);
        hud_quad_->bind();
        glDrawArrays(GL_TRIANGLES, 0, hud_quad_->vertex_count());
        glViewport(0, 0, screen_width_, screen_height_);
    }

    glActiveTexture(GL_TEXTURE0);
}

void Engine::update_engine(float dt) {
    CosmosConfig& config = CosmosConfig::get();

    // Camera physics
    camera_.collidable()->update(dt);

    // Collision: camera vs all GameObjects with collidables
    if (config.is_collisions()) {
        for (const auto& obj : GameObjectManager::get().objects()) {
            if (obj->get_collidable()) {
                camera_.collidable()->check(obj->get_collidable());
            }
        }
    }

    // Audio listener follows camera
    AudioManager::get().set_listener_transform(camera_.transform());
}
