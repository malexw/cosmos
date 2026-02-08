#ifndef COSMOS_TERRAINDATA_HPP_
#define COSMOS_TERRAINDATA_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "InstanceGroup.hpp"
#include "Mesh.hpp"
#include "TerrainDefinition.hpp"
#include "util.hpp"

class TerrainData {
public:
    typedef std::shared_ptr<TerrainData> ShPtr;

    TerrainData();

    void build_from_definition(const TerrainDefinition& def);
    void render();
    void render_shadow(const glm::mat4& lightVP);

    // Runtime generation API for procedural terrain
    void begin_generation();
    void add_instance(const std::string& mesh_name, const glm::mat4& transform);
    void end_generation();

    // Register a mesh for runtime generation
    void register_mesh(const std::string& name, Mesh::ShPtr mesh);

private:
    // Loaded meshes by name
    std::unordered_map<std::string, Mesh::ShPtr> meshes_;

    // Instance groups for batched rendering
    std::vector<InstanceGroup::ShPtr> groups_;

    // Temporary storage during generation
    struct PendingInstance {
        std::string mesh;
        glm::mat4 transform;
    };
    std::vector<PendingInstance> pending_instances_;
    bool generating_;

    // Group key for hashing (mesh ptr, material ptr, submesh index)
    struct GroupKey {
        Mesh* mesh;
        Material* material;
        int submesh_index;

        bool operator==(const GroupKey& other) const {
            return mesh == other.mesh && material == other.material &&
                   submesh_index == other.submesh_index;
        }
    };

    struct GroupKeyHash {
        size_t operator()(const GroupKey& k) const {
            size_t h1 = std::hash<void*>()(k.mesh);
            size_t h2 = std::hash<void*>()(k.material);
            size_t h3 = std::hash<int>()(k.submesh_index);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };

    void build_groups();

    DISALLOW_COPY_AND_ASSIGN(TerrainData);
};

#endif
