#include <iostream>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Engine.hpp"
#include "CosmosConfig.hpp"
#include "GameObjectManager.hpp"
#include "GameScript.hpp"
#include "ResourceManager/AudioManager.hpp"
#include "ResourceManager/MeshManager.hpp"
#include "ResourceManager/ResourceManager.hpp"
#include "ResourceManager/ShaderManager.hpp"
#include "ResourceManager/TextureManager.hpp"
#include "Vector3f.hpp"

Engine::Engine(int width, int height, const char* title)
    : screen_width_(width),
      screen_height_(height),
      window_(nullptr),
      gl_context_(nullptr),
      shadow_buffer_(0),
      hdr_frame_buffer_(0),
      hdr_depth_rb_(0),
      hdr_enabled_(false),
      hdr_headroom_(1.0f),
      sdr_white_level_(1.0f),
      light_dir_(0.0f),
      light_ambient_(0.0f),
      light_diffuse_(0.0f),
      light_specular_(0.0f),
      ambient_global_(0.0f) {

    srand(31337);

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL_Init failed: " << SDL_GetError();
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_FLOATBUFFERS, 1);

    window_ = SDL_CreateWindow(
        title,
        screen_width_, screen_height_,
        SDL_WINDOW_OPENGL
    );
    if (window_ == nullptr) {
        std::cout << "SDL_CreateWindow failed: " << SDL_GetError();
        SDL_Quit();
        return;
    }

    gl_context_ = SDL_GL_CreateContext(window_);
    if (gl_context_ == nullptr) {
        std::cout << "SDL_GL_CreateContext failed: " << SDL_GetError();
        SDL_DestroyWindow(window_);
        window_ = nullptr;
        SDL_Quit();
        return;
    }

    SDL_RaiseWindow(window_);

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
}

Engine::~Engine() {
    if (shadow_buffer_) {
        glDeleteFramebuffers(1, &shadow_buffer_);
    }
    if (hdr_frame_buffer_) {
        glDeleteFramebuffers(1, &hdr_frame_buffer_);
    }
    if (hdr_depth_rb_) {
        glDeleteRenderbuffers(1, &hdr_depth_rb_);
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
    // Shadow FBO
    glGenFramebuffers(1, &shadow_buffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer_);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        TextureManager::get().get_texture("shadow_map")->get_index(), 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // HDR FBO
    glGenFramebuffers(1, &hdr_frame_buffer_);
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_frame_buffer_);
    GLuint hdr_tex = TextureManager::get().get_texture("hdr target")->get_index();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, hdr_tex, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Resize HDR color target to match window
    glBindTexture(GL_TEXTURE_2D, hdr_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screen_width_, screen_height_, 0, GL_RGBA, GL_FLOAT, nullptr);

    // Attach depth renderbuffer
    glGenRenderbuffers(1, &hdr_depth_rb_);
    glBindRenderbuffer(GL_RENDERBUFFER, hdr_depth_rb_);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, screen_width_, screen_height_);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdr_depth_rb_);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::run(GameScript& game) {
    if (!window_ || !gl_context_) return;

    init_fbos();
    game.init(*this);

    const float target_frame_time = 1.0f / 60.0f;

    while (true) {
        timer_.frame_start();
        input_manager_.handleInput();

        float dt = timer_.frame_delta();
        if (!game.update(dt)) break;

        update_engine(dt);

        // Query HDR display state
        SDL_PropertiesID props = SDL_GetWindowProperties(window_);
        hdr_enabled_ = SDL_GetBooleanProperty(props, SDL_PROP_WINDOW_HDR_ENABLED_BOOLEAN, false);
        hdr_headroom_ = SDL_GetFloatProperty(props, SDL_PROP_WINDOW_HDR_HEADROOM_FLOAT, 1.0f);
        sdr_white_level_ = SDL_GetFloatProperty(props, SDL_PROP_WINDOW_SDR_WHITE_LEVEL_FLOAT, 1.0f);

        render();
        SDL_GL_SwapWindow(window_);

        timer_.frame_stop();
        float elapsed = timer_.frame_length();
        if (elapsed < target_frame_time) {
            SDL_Delay(static_cast<Uint32>((target_frame_time - elapsed) * 1000.0f));
        }
    }

    game.shutdown();
}

void Engine::render() {
    CosmosConfig& config = CosmosConfig::get();
    int width = screen_width_;
    int height = screen_height_;

    // GL state from config
    if (!config.is_textures()) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // --- Shadow pass ---
    if (shadow_camera_.enabled() && config.is_shadows()) {
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_buffer_);
        glViewport(0, 0, 2048, 2048);
        glClear(GL_DEPTH_BUFFER_BIT);
        glCullFace(GL_FRONT);
        glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

        glm::mat4 shadow_view = shadow_camera_.view_matrix();
        glm::mat4 shadow_proj = shadow_camera_.projection();

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

                flatProg->setMat4("mvp", shadow_proj * shadow_view * model);
                tile.mesh->bind();
                glDrawArrays(GL_TRIANGLES, 0, tile.mesh->vertex_count());
            }
        }
        glBindVertexArray(0);

        // Draw renderable game objects with transforms
        for (const auto& obj : GameObjectManager::get().objects()) {
            if (obj->get_transform() && obj->get_renderable()) {
                flatProg->setMat4("mvp", shadow_proj * shadow_view * obj->get_transform()->get_matrix());
                obj->get_renderable()->draw_geometry();
            }
        }

        // Draw instanced terrain shadows
        if (terrain_) {
            terrain_->render_shadow(shadow_proj * shadow_view);
        }
    }

    // --- HDR pass: all rendering goes to hdr_frame_buffer_ ---
    glBindFramebuffer(GL_FRAMEBUFFER, hdr_frame_buffer_);
    glViewport(0, 0, width, height);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);
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

    // Compute light direction in eye space
    glm::vec3 lightPosEye = glm::mat3(mainView) * light_dir_;

    // Upload per-frame UBO for bumpdec/blinn shaders
    glm::mat4 shadowMatrix(0.0f);
    if (shadow_camera_.enabled() && config.is_shadows()) {
        shadowMatrix = shadow_camera_.tex_matrix();
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D,
            TextureManager::get().get_texture("shadow_map")->get_index());
        glActiveTexture(GL_TEXTURE0);
    }
    if (config.is_textures()) {
        ShaderManager::get().set_per_frame(mainProj, mainView, lightPosEye, shadowMatrix);

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

    // Shadow debug frustum wireframe
    if (config.is_shadow_debug() && shadow_camera_.enabled()) {
        shadow_camera_.render_frustum(mainPV);
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

    // --- Resolve pass: tone-map HDR FBO to screen ---
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);

    auto resolveProg = ShaderManager::get().get_shader_program("resolve");
    resolveProg->run();
    resolveProg->seti("hdr_output", hdr_enabled_ ? 1 : 0);
    resolveProg->setf("sdr_white", sdr_white_level_);
    resolveProg->setf("hdr_headroom", hdr_headroom_);
    resolveProg->setf("exposure", config.exposure());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, TextureManager::get().get_texture("hdr target")->get_index());
    glm::mat4 resolveProj = glm::ortho(-0.5f, 0.5f, -0.5f, 0.5f, -1.0f, 1.0f);
    resolveProg->setMat4("mvp", resolveProj);
    hud_quad_->bind();
    glDrawArrays(GL_TRIANGLES, 0, hud_quad_->vertex_count());

    glEnable(GL_DEPTH_TEST);
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
