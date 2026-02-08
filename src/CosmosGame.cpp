#include <iostream>

#include "CosmosGame.hpp"
#include "Camera.hpp"
#include "Engine.hpp"
#include "CosmosConfig.hpp"
#include "GameObjectManager.hpp"
#include "PlayerInputHandler.hpp"
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

    // Input handler (game-specific, not in scene file)
    Camera& cam = engine.camera();
    PlayerInputHandler::ShPtr pih(
        new PlayerInputHandler(cam.transform(), cam.collidable()));
    InputHandler::ShPtr ih(std::dynamic_pointer_cast<InputHandler>(pih));
    engine.input_manager().pushHandler(ih);
}

bool CosmosGame::update(float dt) {
    CosmosConfig& config = CosmosConfig::get();

    if (!config.is_valid()) {
        if (config.is_quit()) {
            return false;
        }
        if (!config.is_sounds()) {
            part_sound_->pause();
        } else {
            part_sound_->play();
        }

        config.set_valid();
    }

    // Physics
    emitter_->update(dt);
    campfire_->update(dt);
    rotation_angle_ += 1.0f;

    // Update cube rotation via Transform
    Quaternion q = Quaternion(Vector3f::UNIT_Y, rotation_angle_)
                 * Quaternion(Vector3f::UNIT_X, rotation_angle_ / 2.0f);
    spinning_cube_->get_transform()->set_quat(q);

    return true;
}

void CosmosGame::shutdown() {
}
