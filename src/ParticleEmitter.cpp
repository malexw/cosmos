#include "ParticleEmitter.hpp"

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#endif

#include <glm/gtc/type_ptr.hpp>

#include "ResourceManager/MeshManager.hpp"
#include "ResourceManager/ResourceManager.hpp"
#include "ResourceManager/ShaderManager.hpp"

ParticleEmitter::ParticleEmitter(ParticleEmitterDef::ShPtr def, Vector3f position, Vector3f direction, Vector3f up)
    : def_(def), pos_(position), norm_(direction), up_(up),
      part_count_(static_cast<int>(def->emission_rate * def->lifetime) + 1),
      generation_rate_(1.0f / def->emission_rate),
      generation_remainder_(1.0f / def->emission_rate),
      next_part_(0),
      particle_vao_(0), instance_vbo_(0),
      max_particles_(static_cast<int>(def->emission_rate * def->lifetime) + 1) {

    // Resolve mesh and texture from resource manager
    mesh_ = MeshManager::get().get_mesh(def_->mesh_path);
    if (!mesh_) {
        mesh_ = Mesh::create_quad();
    }
    texture_ = ResourceManager::get().get_texture(def_->texture_path);

    init();
}

ParticleEmitter::~ParticleEmitter() {
    if (instance_vbo_) glDeleteBuffers(1, &instance_vbo_);
    if (particle_vao_) glDeleteVertexArrays(1, &particle_vao_);
}

void ParticleEmitter::init() {
    // Create particle pool
    for (int i = 0; i < part_count_; ++i) {
        Particle::ShPtr p(new Particle());
        parts_.push_back(p);
    }

    // Set up instanced VAO
    glGenVertexArrays(1, &particle_vao_);
    glBindVertexArray(particle_vao_);

    // Bind mesh's VBO for per-vertex attributes
    if (mesh_) {
        glBindBuffer(GL_ARRAY_BUFFER, mesh_->vbo());
        // location 0 = position (vec3)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,
            reinterpret_cast<GLvoid*>(mesh_->vertex_offset()));
        // location 1 = texCoord (vec2)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0,
            reinterpret_cast<GLvoid*>(mesh_->texcoord_offset()));
    }

    // Create and set up instance VBO
    glGenBuffers(1, &instance_vbo_);
    glBindBuffer(GL_ARRAY_BUFFER, instance_vbo_);
    glBufferData(GL_ARRAY_BUFFER, max_particles_ * sizeof(ParticleInstance), nullptr, GL_DYNAMIC_DRAW);

    // location 6 = instancePosition (vec3, stride=32, offset=0), divisor=1
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 3, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance),
        reinterpret_cast<GLvoid*>(0));
    glVertexAttribDivisor(6, 1);

    // location 7 = instanceColor (vec4, stride=32, offset=12), divisor=1
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance),
        reinterpret_cast<GLvoid*>(12));
    glVertexAttribDivisor(7, 1);

    // location 8 = instanceScale (float, stride=32, offset=28), divisor=1
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(ParticleInstance),
        reinterpret_cast<GLvoid*>(28));
    glVertexAttribDivisor(8, 1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ParticleEmitter::update(float delta) {
    Vector3f gravity(def_->gravity.x, def_->gravity.y, def_->gravity.z);

    for (const Particle::ShPtr& part : parts_) {
        part->update(delta, gravity);
    }

    int new_part_count = delta / generation_rate_;
    generation_remainder_ -= fmod(delta, generation_rate_);
    if (generation_remainder_ < 0) {
        generation_remainder_ += generation_rate_;
        new_part_count += 1;
    }

    for (int i = 0; i < new_part_count; ++i) {
        next_part_ = (next_part_ + i + 1) % part_count_;
        float angle = (fmod(rand(), (def_->spread * 2000)) / 1000.0f) - def_->spread;
        float dir = (rand() % 180000) / 1000.0f;
        Quaternion deflection(up_.cross(norm_), angle);
        Quaternion rotation(norm_, dir);
        parts_.at(next_part_)->reset(pos_, def_->speed * (rotation * (deflection * norm_)), def_->lifetime);
    }
}

void ParticleEmitter::render(Transform::ShPtr cam, const glm::mat4& projView) {
    if (!mesh_ || !texture_) return;

    // Build instance data from alive particles
    std::vector<ParticleInstance> instances;
    instances.reserve(max_particles_);

    for (const Particle::ShPtr& part : parts_) {
        if (!part->is_alive()) continue;

        float t = part->age_normalized();
        glm::vec4 color = glm::mix(def_->start_color, def_->end_color, t);
        float scale = def_->start_size + t * (def_->end_size - def_->start_size);

        ParticleInstance inst;
        inst.position = glm::vec3(part->position().x(), part->position().y(), part->position().z());
        inst.color = color;
        inst.scale = scale;
        instances.push_back(inst);
    }

    if (instances.empty()) return;

    // Upload instance data
    glBindBuffer(GL_ARRAY_BUFFER, instance_vbo_);
    glBufferSubData(GL_ARRAY_BUFFER, 0,
        instances.size() * sizeof(ParticleInstance), instances.data());

    // Get camera right and up vectors for billboarding
    Vector3f camRight = cam->get_rotation() * Vector3f::UNIT_X;
    Vector3f camUp = cam->get_rotation() * Vector3f::UNIT_Y;

    // Bind particle shader
    auto particleProg = ShaderManager::get().get_shader_program("particle");
    particleProg->run();
    particleProg->setMat4("viewProj", projView);

    GLint rightLoc = glGetUniformLocation(particleProg->get_id(), "cameraRight");
    if (rightLoc >= 0) glUniform3f(rightLoc, camRight.x(), camRight.y(), camRight.z());
    GLint upLoc = glGetUniformLocation(particleProg->get_id(), "cameraUp");
    if (upLoc >= 0) glUniform3f(upLoc, camUp.x(), camUp.y(), camUp.z());

    // Bind texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_->get_index());

    // Draw instanced
    glBindVertexArray(particle_vao_);
    glDrawArraysInstanced(GL_TRIANGLES, 0, mesh_->vertex_count(), instances.size());
    glBindVertexArray(0);
}

void ParticleEmitter::rotate(const Vector3f& axis, float angle) {
    Quaternion q(axis, angle);
    norm_ = q * norm_;
    up_ = q * up_;
}
