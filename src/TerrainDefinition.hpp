#ifndef COSMOS_TERRAINDEFINITION_HPP_
#define COSMOS_TERRAINDEFINITION_HPP_

#include <string>
#include <vector>

#include <glm/glm.hpp>

struct TerrainDefinition {
    struct MeshRef {
        std::string name;
        std::string uri;
    };

    struct InstanceDef {
        std::string mesh;
        glm::mat4 transform;
    };

    std::vector<MeshRef> meshes;
    std::vector<InstanceDef> instances;
};

#endif
