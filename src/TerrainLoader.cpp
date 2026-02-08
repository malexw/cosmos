#include "TerrainLoader.hpp"

#include <fstream>
#include <iostream>

#include "json.hpp"

using json = nlohmann::json;

TerrainDefinition TerrainLoader::load_definition(const std::string& path) {
    TerrainDefinition def;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "TerrainLoader: could not open " << path << std::endl;
        return def;
    }

    json data;
    try {
        file >> data;
    } catch (const json::parse_error& e) {
        std::cout << "TerrainLoader: JSON parse error in " << path << ": " << e.what() << std::endl;
        return def;
    }

    // Parse meshes array
    if (data.contains("meshes") && data["meshes"].is_array()) {
        for (const auto& mesh_json : data["meshes"]) {
            TerrainDefinition::MeshRef mesh_ref;
            if (mesh_json.contains("name")) {
                mesh_ref.name = mesh_json["name"].get<std::string>();
            }
            if (mesh_json.contains("uri")) {
                mesh_ref.uri = mesh_json["uri"].get<std::string>();
            }
            def.meshes.push_back(mesh_ref);
        }
    }

    // Parse instances array
    if (data.contains("instances") && data["instances"].is_array()) {
        for (const auto& inst_json : data["instances"]) {
            TerrainDefinition::InstanceDef inst;

            if (inst_json.contains("mesh")) {
                inst.mesh = inst_json["mesh"].get<std::string>();
            }

            // Parse transform as column-major 4x4 matrix (16 floats)
            inst.transform = glm::mat4(1.0f);
            if (inst_json.contains("transform") && inst_json["transform"].is_array()) {
                const auto& t = inst_json["transform"];
                if (t.size() == 16) {
                    // Column-major order: columns[0-3], each column has 4 elements
                    inst.transform[0][0] = t[0].get<float>();
                    inst.transform[0][1] = t[1].get<float>();
                    inst.transform[0][2] = t[2].get<float>();
                    inst.transform[0][3] = t[3].get<float>();

                    inst.transform[1][0] = t[4].get<float>();
                    inst.transform[1][1] = t[5].get<float>();
                    inst.transform[1][2] = t[6].get<float>();
                    inst.transform[1][3] = t[7].get<float>();

                    inst.transform[2][0] = t[8].get<float>();
                    inst.transform[2][1] = t[9].get<float>();
                    inst.transform[2][2] = t[10].get<float>();
                    inst.transform[2][3] = t[11].get<float>();

                    inst.transform[3][0] = t[12].get<float>();
                    inst.transform[3][1] = t[13].get<float>();
                    inst.transform[3][2] = t[14].get<float>();
                    inst.transform[3][3] = t[15].get<float>();
                }
            }

            def.instances.push_back(inst);
        }
    }

    std::cout << "TerrainLoader: loaded " << def.meshes.size() << " meshes, "
              << def.instances.size() << " instances from " << path << std::endl;

    return def;
}

TerrainData::ShPtr TerrainLoader::instantiate(const TerrainDefinition& def) {
    TerrainData::ShPtr terrain(new TerrainData());
    terrain->build_from_definition(def);
    return terrain;
}

TerrainData::ShPtr TerrainLoader::load(const std::string& path) {
    TerrainDefinition def = load_definition(path);
    return instantiate(def);
}
