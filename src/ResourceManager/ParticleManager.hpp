#ifndef COSMOS_PARTICLEMANAGER_HPP_
#define COSMOS_PARTICLEMANAGER_HPP_

#include <memory>
#include <string>
#include <unordered_map>

#include "ParticleEmitterDef.hpp"
#include "ParticleEmitter.hpp"
#include "util.hpp"
#include "Vector3f.hpp"

class ParticleManager {
public:
    typedef std::shared_ptr<ParticleManager> ShPtr;
    ParticleManager();
    static ParticleManager& get();

    void store_emitter_def(const std::string& name, ParticleEmitterDef::ShPtr def);
    ParticleEmitterDef::ShPtr get_emitter_def(const std::string& name) const;

    ParticleEmitter::ShPtr create_emitter(
        const std::string& def_path,
        const Vector3f& position,
        const Vector3f& direction,
        const Vector3f& up = Vector3f::UNIT_X);

private:
    std::unordered_map<std::string, ParticleEmitterDef::ShPtr> defs_;
    void init();
    DISALLOW_COPY_AND_ASSIGN(ParticleManager);
};

#endif
