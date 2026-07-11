#include <iostream>

#include <imgui.h>

#include "CosmosGame.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "CosmosConfig.hpp"
#include "GameObjectManager.hpp"
#include "InputManager.hpp"
#include "Quaternion.hpp"
#include "SceneLoader.hpp"
#include "ResourceManager/MaterialManager.hpp"
#include "ResourceManager/MeshManager.hpp"
#include "Vector3f.hpp"

CosmosGame::CosmosGame()
    : engine_(nullptr),
      rotation_angle_(0.0f) {
}

void CosmosGame::init(Engine& engine) {
    engine_ = &engine;

    SceneDefinition scene = SceneLoader::load("res/scenes/campground.tres");
    SceneInstances instances = SceneLoader::instantiate(scene, engine);

    // Grab references for per-frame game logic
    skybox_ = instances.skybox;
    MeshManager::get().get_mesh("res/meshes/hdrbox.obj")
        ->set_material(MaterialManager::get().get_material("res/materials/hdrbox.mtl"));
    spinning_cube_ = GameObjectManager::get().get_object_by_name("spinning_cube");
    cube_collidable_ = spinning_cube_->get_collidable();
    emitter_ = instances.emitters["ion_fountain"];
    campfire_ = instances.emitters["campfire"];
    part_sound_ = instances.sounds["engine_hum"];

    // Player controller
    Camera& cam = engine.camera();
    player_controller_ = std::make_unique<PlayerController>(
        engine.window(), cam.transform(), cam.collidable());
}

bool CosmosGame::update(float dt) {
    CosmosConfig& config = CosmosConfig::get();
    const InputState& input = engine_->input_manager().input_state();
    const ActionState& actions = engine_->input_manager().action_state();

    // Quit
    if (actions.just_triggered(Action::Quit) || input.quit_requested()) {
        return false;
    }

    // Sound toggle
    if (!config.is_valid()) {
        if (!config.is_sounds()) {
            part_sound_->pause();
        } else {
            part_sound_->play();
        }
        config.set_valid();
    }

    // Player controller
    player_controller_->update(actions);

    // Physics
    emitter_->update(dt);
    campfire_->update(dt);
    rotation_angle_ += 1.0f;

    // Update cube rotation via Transform
    Quaternion q = Quaternion(Vector3f::UNIT_Y, rotation_angle_)
                 * Quaternion(Vector3f::UNIT_X, rotation_angle_ / 2.0f);
    spinning_cube_->get_transform()->set_quat(q);

    // Debug menu toggle
    if (actions.just_triggered(Action::ToggleDebugMenu)) {
        debug_menu_open_ = !debug_menu_open_;
    }

    // ImGui debug panel
    if (debug_menu_open_) {
        ImGui::Begin("Cosmos Debug", &debug_menu_open_);

        if (ImGui::CollapsingHeader("Rendering", ImGuiTreeNodeFlags_DefaultOpen)) {
            bool textures = config.is_textures();
            if (ImGui::Checkbox("Textures", &textures)) config.set_textures(textures);

            bool bump = config.is_bump_mapping();
            if (ImGui::Checkbox("Bump Mapping", &bump)) config.set_bump_mapping(bump);

            bool decals = config.is_decals();
            if (ImGui::Checkbox("Decals", &decals)) config.set_decals(decals);

            bool skybox = config.is_skybox();
            if (ImGui::Checkbox("Skybox", &skybox)) config.set_skybox(skybox);

            bool particles = config.is_particles();
            if (ImGui::Checkbox("Particles", &particles)) config.set_particles(particles);

            bool ssao = config.is_ssao();
            if (ImGui::Checkbox("SSAO", &ssao)) config.set_ssao(ssao);

            if (config.is_ssao()) {
                float ssaoRadius = config.ssao_radius();
                if (ImGui::SliderFloat("SSAO Radius", &ssaoRadius, 0.1f, 2.0f)) config.set_ssao_radius(ssaoRadius);

                float ssaoBias = config.ssao_bias();
                if (ImGui::SliderFloat("SSAO Bias", &ssaoBias, 0.01f, 0.5f)) config.set_ssao_bias(ssaoBias);

                float ssaoPower = config.ssao_power();
                if (ImGui::SliderFloat("SSAO Power", &ssaoPower, 0.5f, 4.0f)) config.set_ssao_power(ssaoPower);
            }

            bool axes = config.is_show_axes();
            if (ImGui::Checkbox("Show Axes", &axes)) config.set_show_axes(axes);
        }

        if (ImGui::CollapsingHeader("Shadows")) {
            bool shadows = config.is_shadows();
            if (ImGui::Checkbox("Enabled", &shadows)) config.set_shadows(shadows);

            bool shadowDbg = config.is_shadow_debug();
            if (ImGui::Checkbox("Shadow Debug", &shadowDbg)) config.set_shadow_debug(shadowDbg);

            int pcf = config.pcf_mode();
            const char* pcfItems[] = { "Off", "Basic", "Poisson" };
            if (ImGui::Combo("PCF Mode", &pcf, pcfItems, 3)) config.set_pcf_mode(pcf);

            float shadowDist = config.shadow_distance();
            if (ImGui::SliderFloat("Shadow Distance", &shadowDist, 10.0f, 200.0f))
                config.set_shadow_distance(shadowDist);

            int cascades = config.shadow_cascades();
            if (ImGui::SliderInt("Cascades", &cascades, 1, 4))
                config.set_shadow_cascades(cascades);

            float shadowBias = config.shadow_bias();
            if (ImGui::SliderFloat("Shadow Bias", &shadowBias, 0.001f, 0.5f))
                config.set_shadow_bias(shadowBias);

            bool cubeDbg = config.is_shadow_cube_debug();
            if (ImGui::Checkbox("Cube Map Debug", &cubeDbg)) config.set_shadow_cube_debug(cubeDbg);

            if (cubeDbg) {
                int cubeFace = config.shadow_cube_face();
                const char* faceItems[] = { "+X", "-X", "+Y", "-Y", "+Z", "-Z" };
                if (ImGui::Combo("Cube Face", &cubeFace, faceItems, 6))
                    config.set_shadow_cube_face(cubeFace);
            }
        }

        if (ImGui::CollapsingHeader("Physics")) {
            bool collisions = config.is_collisions();
            if (ImGui::Checkbox("Collisions", &collisions)) config.set_collisions(collisions);

            bool collidables = config.is_collidables();
            if (ImGui::Checkbox("Show Collidables", &collidables)) config.set_collidables(collidables);
        }

        if (ImGui::CollapsingHeader("Audio")) {
            bool sounds = config.is_sounds();
            if (ImGui::Checkbox("Sounds", &sounds)) config.set_sounds(sounds);
        }

        if (ImGui::CollapsingHeader("Display")) {
            ImGui::Text("Resolution: %dx%d", engine_->screen_width(), engine_->screen_height());
            ImGui::Text("Framebuffer: %s", engine_->float_framebuffer() ? "Float (HDR)" : "Integer (SDR)");
        }

        if (ImGui::CollapsingHeader("Tone Mapping", ImGuiTreeNodeFlags_DefaultOpen)) {
            float exposure = config.exposure();
            if (ImGui::SliderFloat("Exposure", &exposure, 0.1f, 20.0f)) config.set_exposure(exposure);

            ImGui::Text("Presets:");
            ImGui::SameLine();
            if (ImGui::SmallButton("1x")) config.set_exposure(1.0f);
            ImGui::SameLine();
            if (ImGui::SmallButton("2x")) config.set_exposure(2.0f);
            ImGui::SameLine();
            if (ImGui::SmallButton("6x")) config.set_exposure(6.0f);
            ImGui::SameLine();
            if (ImGui::SmallButton("8x")) config.set_exposure(8.0f);
            ImGui::SameLine();
            if (ImGui::SmallButton("10x")) config.set_exposure(10.0f);
        }

        ImGui::Separator();
        ImGui::Text("%.1f FPS (%.3f ms/frame)", ImGui::GetIO().Framerate, 1000.0f / ImGui::GetIO().Framerate);

        ImGui::End();
    }

    return true;
}

void CosmosGame::shutdown() {
}
