#ifndef COSMOS_COSMOSGAME_HPP_
#define COSMOS_COSMOSGAME_HPP_

#include <memory>

#include "GameScript.hpp"
#include "GameObject.hpp"
#include "CollidableObject.hpp"
#include "ParticleEmitter.hpp"
#include "PlayerController.hpp"
#include "Renderable.hpp"
#include "SkyBox.hpp"
#include "Sound.hpp"

class CosmosGame : public GameScript {
public:
    CosmosGame();

    void init(Engine& engine) override;
    bool update(float dt) override;
    void shutdown() override;

private:
    Engine* engine_;

    // Game objects
    SkyBox::ShPtr skybox_;
    GameObject::ShPtr spinning_cube_;

    // Component refs
    CollidableObject::ShPtr cube_collidable_;
    ParticleEmitter::ShPtr emitter_;
    ParticleEmitter::ShPtr campfire_;
    Sound::ShPtr part_sound_;

    // Game state
    float rotation_angle_;

    // Debug menu
    bool debug_menu_open_ = false;

    std::unique_ptr<PlayerController> player_controller_;
};

#endif
