#include <iostream>
#include <memory>

#include "ParticleManager.hpp"

ParticleManager::ParticleManager() {
    init();
}

void ParticleManager::init() {
}

ParticleManager& ParticleManager::get() {
    static ParticleManager instance;
    return instance;
}

void ParticleManager::store_emitter_def(const std::string& name, ParticleEmitterDef::ShPtr def) {
    defs_[name] = def;
}

ParticleEmitterDef::ShPtr ParticleManager::get_emitter_def(const std::string& name) const {
    auto it = defs_.find(name);
    if (it != defs_.end()) return it->second;
    std::cout << "ParticleManager: emitter def <" << name << "> not found" << std::endl;
    return ParticleEmitterDef::ShPtr();
}

ParticleEmitter::ShPtr ParticleManager::create_emitter(
    const std::string& def_path,
    const Vector3f& position,
    const Vector3f& direction,
    const Vector3f& up) {

    ParticleEmitterDef::ShPtr def = get_emitter_def(def_path);
    if (!def) return ParticleEmitter::ShPtr();

    auto emitter = std::make_shared<ParticleEmitter>(def, position, direction, up);
    return emitter;
}
