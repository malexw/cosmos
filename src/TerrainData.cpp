#include "TerrainData.hpp"

#include <iostream>
#include <memory>
#include <unordered_map>

#include "CosmosConfig.hpp"
#include "GLDebug.hpp"
#include "ResourceManager/MeshManager.hpp"
#include "ResourceManager/ShaderManager.hpp"

TerrainData::TerrainData() : generating_(false) {
}

void TerrainData::build_from_definition(const TerrainDefinition& def) {
    // Load all meshes referenced in the definition
    for (const auto& mesh_ref : def.meshes) {
        Mesh::ShPtr mesh = MeshManager::get().load_mesh(mesh_ref.uri);
        if (mesh) {
            meshes_[mesh_ref.name] = mesh;
        } else {
            std::cout << "TerrainData: failed to load mesh " << mesh_ref.uri << std::endl;
        }
    }

    // Convert instances to pending format and build groups
    pending_instances_.clear();
    for (const auto& inst : def.instances) {
        PendingInstance pi;
        pi.mesh = inst.mesh;
        pi.transform = inst.transform;
        pending_instances_.push_back(pi);
    }

    build_groups();
}

void TerrainData::render() {
    if (groups_.empty()) return;

    CosmosConfig& config = CosmosConfig::get();
    if (!config.is_textures()) return;

    auto texturedProg = ShaderManager::get().get_shader_program("bumpdec_instanced");
    auto untexturedProg = ShaderManager::get().get_shader_program("blinn_instanced");
    if (!texturedProg || !untexturedProg) {
        std::cout << "TerrainData: instanced shaders not found" << std::endl;
        return;
    }

    bool shadowDbg = config.is_shadow_debug();
    int pcfMode = config.pcf_mode();

    for (const auto& group : groups_) {
        Material::ShPtr mat = group->material();
        bool textured = mat && mat->is_textured();

        // Choose shader based on whether material has textures
        auto prog = textured ? texturedProg : untexturedProg;
        prog->run();

        // Set shadow uniforms
        prog->seti(std::string("debug_shadows"), shadowDbg ? 1 : 0);
        prog->seti(std::string("pcf_mode"), pcfMode);

        if (textured) {
            // Set texture-specific uniforms
            GLint loc;
            loc = glGetUniformLocation(prog->get_id(), "has_bump_map");
            if (loc >= 0) glUniform1i(loc, 0);
            loc = glGetUniformLocation(prog->get_id(), "has_decal");
            if (loc >= 0) glUniform1i(loc, 0);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mat->get_texture()->get_index());

            bool use_bump = mat->is_bump_mapped() && config.is_bump_mapping();
            bool use_decal = use_bump && mat->is_decal_mapped() && config.is_decals();

            loc = glGetUniformLocation(prog->get_id(), "has_bump_map");
            if (loc >= 0) glUniform1i(loc, use_bump ? 1 : 0);

            loc = glGetUniformLocation(prog->get_id(), "has_decal");
            if (loc >= 0) glUniform1i(loc, use_decal ? 1 : 0);

            if (use_bump) {
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, mat->get_bump_tex()->get_index());
            }
            if (use_decal) {
                glActiveTexture(GL_TEXTURE2);
                glBindTexture(GL_TEXTURE_2D, mat->get_decal_tex()->get_index());
            }
        }

        group->draw_instanced();
    }

    glActiveTexture(GL_TEXTURE0);
}

void TerrainData::render_shadow(const glm::mat4& lightVP) {
    if (groups_.empty()) return;

    auto flatInst = ShaderManager::get().get_shader_program("flat_instanced");
    if (!flatInst) {
        std::cout << "TerrainData: flat_instanced shader not found" << std::endl;
        return;
    }

    flatInst->run();
    flatInst->setMat4("lightViewProj", lightVP);

    // Draw all groups - each has its own instance buffer with transforms
    for (const auto& group : groups_) {
        group->draw_instanced_shadow();
    }
}

void TerrainData::begin_generation() {
    generating_ = true;
    pending_instances_.clear();
    groups_.clear();
}

void TerrainData::add_instance(const std::string& mesh_name, const glm::mat4& transform) {
    if (!generating_) {
        std::cout << "TerrainData: add_instance called outside of generation" << std::endl;
        return;
    }

    PendingInstance pi;
    pi.mesh = mesh_name;
    pi.transform = transform;
    pending_instances_.push_back(pi);
}

void TerrainData::end_generation() {
    if (!generating_) return;

    build_groups();
    generating_ = false;
}

void TerrainData::register_mesh(const std::string& name, Mesh::ShPtr mesh) {
    meshes_[name] = mesh;
}

void TerrainData::build_groups() {
    groups_.clear();

    // Group instances by (mesh, material, submesh_index)
    std::unordered_map<GroupKey, InstanceGroup::ShPtr, GroupKeyHash> group_map;

    for (const auto& pi : pending_instances_) {
        auto it = meshes_.find(pi.mesh);
        if (it == meshes_.end()) {
            std::cout << "TerrainData: unknown mesh " << pi.mesh << std::endl;
            continue;
        }

        Mesh::ShPtr mesh = it->second;
        const auto& submeshes = mesh->submeshes();

        for (int i = 0; i < static_cast<int>(submeshes.size()); ++i) {
            Material::ShPtr mat = submeshes[i].material;

            GroupKey key;
            key.mesh = mesh.get();
            key.material = mat.get();
            key.submesh_index = i;

            auto git = group_map.find(key);
            if (git == group_map.end()) {
                auto group = std::make_shared<InstanceGroup>(mesh, mat, i);
                group_map[key] = group;
                git = group_map.find(key);
            }

            git->second->add_instance(pi.transform);
        }
    }

    // Upload all groups
    for (auto& [key, group] : group_map) {
        group->upload_instances();
        groups_.push_back(group);
    }

    pending_instances_.clear();
}
