#ifndef COSMOS_SCENELOADER_HPP_
#define COSMOS_SCENELOADER_HPP_

#include <string>
#include <unordered_map>

#include "ParticleEmitter.hpp"
#include "SceneDefinition.hpp"
#include "SkyBox.hpp"
#include "Sound.hpp"

class Engine;

struct SceneInstances {
    SkyBox::ShPtr skybox;
    std::unordered_map<std::string, ParticleEmitter::ShPtr> emitters;
    std::unordered_map<std::string, Sound::ShPtr> sounds;
};

class SceneLoader {
public:
    static SceneDefinition load(const std::string& path);
    static SceneInstances instantiate(const SceneDefinition& scene, Engine& engine);
};

#endif
