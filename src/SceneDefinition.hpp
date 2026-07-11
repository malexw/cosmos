#ifndef COSMOS_SCENEDEFINITION_HPP_
#define COSMOS_SCENEDEFINITION_HPP_

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "Light.hpp"
#include "Vector3f.hpp"

struct SceneDefinition {
    struct TileDef {
        std::string name;
        char key = ' ';
        std::string mesh;
        float rotation = 0.0f;
    };

    struct GridDef {
        float tile_size = 4.0f;
        std::vector<TileDef> tiles;
        std::vector<std::string> rows;
    };

    struct SkyboxDef {
        std::string mesh;
    };

    struct CameraDef {
        Vector3f position;
        Vector3f direction;
        Vector3f collision_scale;
    };


    struct ObjectDef {
        std::string name;
        std::string mesh;
        Vector3f position;
        std::string collision_type;
        Vector3f collision_scale;
    };

    struct EmitterDef {
        std::string name;
        std::string source;
        Vector3f position;
        Vector3f direction;
        Vector3f up;
    };

    struct SoundDef {
        std::string name;
        std::string source;
        Vector3f position;
        float gain = 1.0f;
        bool looping = false;
        float rolloff = 1.0f;
    };

    struct TerrainRef {
        std::string source;
    };

    GridDef grid;
    TerrainRef terrain;
    SkyboxDef skybox;
    CameraDef camera;
    std::vector<Light> lights;
    std::vector<ObjectDef> objects;
    std::vector<EmitterDef> emitters;
    std::vector<SoundDef> sounds;
};

#endif
