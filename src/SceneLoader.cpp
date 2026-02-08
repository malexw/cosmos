#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "SceneLoader.hpp"
#include "Camera.hpp"
#include "CollidableObject.hpp"
#include "Engine.hpp"
#include "TileGrid.hpp"
#include "TerrainLoader.hpp"
#include "GameObjectManager.hpp"
#include "ResourceManager/AudioManager.hpp"
#include "ResourceManager/MeshManager.hpp"
#include "ResourceManager/ParticleManager.hpp"
#include "ResourceManager/ResourceManager.hpp"

static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t");
    return s.substr(start, end - start + 1);
}

static std::string strip_quotes(const std::string& s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.size() - 2);
    }
    return s;
}

static Vector3f parse_vec3(const std::string& s) {
    std::istringstream ss(s);
    float x = 0, y = 0, z = 0;
    ss >> x >> y >> z;
    return Vector3f(x, y, z);
}

static glm::vec3 parse_glm_vec3(const std::string& s) {
    std::istringstream ss(s);
    float x = 0, y = 0, z = 0;
    ss >> x >> y >> z;
    return glm::vec3(x, y, z);
}

static glm::vec4 parse_glm_vec4(const std::string& s) {
    std::istringstream ss(s);
    float x = 0, y = 0, z = 0, w = 0;
    ss >> x >> y >> z >> w;
    return glm::vec4(x, y, z, w);
}

// Parses a section header line like [type] or [type "name"]
// Returns the section type and optionally the name.
static void parse_section_header(const std::string& line, std::string& type, std::string& name) {
    type.clear();
    name.clear();

    // Strip brackets
    std::string inner = line.substr(1, line.size() - 2);
    inner = trim(inner);

    // Check for quoted name: [type "name"]
    size_t quote_start = inner.find('"');
    if (quote_start != std::string::npos) {
        type = trim(inner.substr(0, quote_start));
        size_t quote_end = inner.find('"', quote_start + 1);
        if (quote_end != std::string::npos) {
            name = inner.substr(quote_start + 1, quote_end - quote_start - 1);
        }
    } else {
        type = inner;
    }
}

SceneDefinition SceneLoader::load(const std::string& path) {
    SceneDefinition scene;

    std::ifstream file(path);
    if (!file.is_open()) {
        std::cout << "SceneLoader: could not open " << path << std::endl;
        return scene;
    }

    std::string current_section;
    std::string current_name;

    // Index into vectors for current object/emitter/sound/tile being parsed
    int current_object = -1;
    int current_emitter = -1;
    int current_sound = -1;
    int current_tile = -1;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();

        std::string trimmed = trim(line);
        if (trimmed.empty() || trimmed[0] == '#') continue;

        // Section header
        if (trimmed[0] == '[') {
            std::string sec_type, sec_name;
            parse_section_header(trimmed, sec_type, sec_name);

            // Skip the resource header
            if (sec_type == "resource") continue;

            current_section = sec_type;
            current_name = sec_name;

            if (sec_type == "tile") {
                SceneDefinition::TileDef td;
                td.name = sec_name;
                scene.grid.tiles.push_back(td);
                current_tile = static_cast<int>(scene.grid.tiles.size()) - 1;
            } else if (sec_type == "object") {
                SceneDefinition::ObjectDef obj;
                obj.name = sec_name;
                scene.objects.push_back(obj);
                current_object = static_cast<int>(scene.objects.size()) - 1;
            } else if (sec_type == "emitter") {
                SceneDefinition::EmitterDef em;
                em.name = sec_name;
                scene.emitters.push_back(em);
                current_emitter = static_cast<int>(scene.emitters.size()) - 1;
            } else if (sec_type == "sound") {
                SceneDefinition::SoundDef snd;
                snd.name = sec_name;
                scene.sounds.push_back(snd);
                current_sound = static_cast<int>(scene.sounds.size()) - 1;
            }
            continue;
        }

        // Key = value
        size_t eq = trimmed.find('=');
        if (eq == std::string::npos) {
            // Bare lines in [grid] are grid rows
            if (current_section == "grid") {
                scene.grid.rows.push_back(trimmed);
            }
            continue;
        }

        std::string key = trim(trimmed.substr(0, eq));
        std::string value = strip_quotes(trim(trimmed.substr(eq + 1)));

        if (current_section == "tile" && current_tile >= 0) {
            auto& td = scene.grid.tiles[current_tile];
            if (key == "key") td.key = value.empty() ? ' ' : value[0];
            else if (key == "mesh") td.mesh = value;
            else if (key == "rotation") td.rotation = std::stof(value);
        } else if (current_section == "grid") {
            if (key == "size") scene.grid.tile_size = std::stof(value);
        } else if (current_section == "skybox") {
            if (key == "mesh") scene.skybox.mesh = value;
        } else if (current_section == "camera") {
            if (key == "position") scene.camera.position = parse_vec3(value);
            else if (key == "direction") scene.camera.direction = parse_vec3(value);
            else if (key == "collision_scale") scene.camera.collision_scale = parse_vec3(value);
        } else if (current_section == "light") {
            if (key == "direction") scene.light.direction = parse_glm_vec3(value);
            else if (key == "ambient") scene.light.ambient = parse_glm_vec4(value);
            else if (key == "diffuse") scene.light.diffuse = parse_glm_vec4(value);
            else if (key == "specular") scene.light.specular = parse_glm_vec4(value);
            else if (key == "ambient_global") scene.light.ambient_global = parse_glm_vec4(value);
        } else if (current_section == "shadow") {
            if (key == "position") scene.shadow.position = parse_glm_vec3(value);
            else if (key == "direction") scene.shadow.direction = parse_glm_vec3(value);
        } else if (current_section == "object" && current_object >= 0) {
            auto& obj = scene.objects[current_object];
            if (key == "mesh") obj.mesh = value;
            else if (key == "position") obj.position = parse_vec3(value);
            else if (key == "collision_type") obj.collision_type = value;
            else if (key == "collision_scale") obj.collision_scale = parse_vec3(value);
        } else if (current_section == "emitter" && current_emitter >= 0) {
            auto& em = scene.emitters[current_emitter];
            if (key == "source") em.source = value;
            else if (key == "position") em.position = parse_vec3(value);
            else if (key == "direction") em.direction = parse_vec3(value);
            else if (key == "up") em.up = parse_vec3(value);
        } else if (current_section == "sound" && current_sound >= 0) {
            auto& snd = scene.sounds[current_sound];
            if (key == "source") snd.source = value;
            else if (key == "position") snd.position = parse_vec3(value);
            else if (key == "gain") snd.gain = std::stof(value);
            else if (key == "looping") snd.looping = (value == "true");
            else if (key == "rolloff") snd.rolloff = std::stof(value);
        } else if (current_section == "terrain") {
            if (key == "source") scene.terrain.source = value;
        }
    }

    return scene;
}

SceneInstances SceneLoader::instantiate(const SceneDefinition& scene, Engine& engine) {
    SceneInstances instances;

    // Skybox
    instances.skybox = SkyBox::ShPtr(new SkyBox());
    instances.skybox->renderable().set_mesh(
        MeshManager::get().get_mesh(scene.skybox.mesh));
    engine.set_skybox(instances.skybox);

    // Camera
    Camera& cam = engine.camera();
    cam.transform()->set_translate(scene.camera.position);
    cam.transform()->set_direction(scene.camera.direction);
    cam.collidable()->set_scale(scene.camera.collision_scale);

    // Game objects
    for (const auto& obj_def : scene.objects) {
        GameObject::ShPtr obj = GameObjectManager::get().create_object(
            obj_def.mesh, obj_def.position, obj_def.name);

        if (!obj_def.collision_type.empty()) {
            unsigned int col_type = CollidableObject::TYPE_SPHERE;
            if (obj_def.collision_type == "capsule") {
                col_type = CollidableObject::TYPE_CAPSULE;
            }
            CollidableObject::ShPtr col(new CollidableObject(obj->id(), col_type));
            col->set_scale(obj_def.collision_scale);
            obj->set_collidable(col);
        }
    }

    // Particle emitters
    for (const auto& em_def : scene.emitters) {
        ResourceManager::get().load_resource(em_def.source);
        ParticleEmitter::ShPtr emitter = ParticleManager::get().create_emitter(
            em_def.source, em_def.position, em_def.direction, em_def.up);
        engine.add_particle_emitter(emitter);
        instances.emitters[em_def.name] = emitter;
    }

    // Sounds
    for (const auto& snd_def : scene.sounds) {
        Sound::ShPtr sound = AudioManager::get().load_sound(snd_def.source);
        sound->set_position(snd_def.position);
        sound->set_gain(snd_def.gain);
        sound->set_looping(snd_def.looping);
        sound->set_rolloff(snd_def.rolloff);
        instances.sounds[snd_def.name] = sound;
    }

    // Tile grid
    TileGrid tile_grid;
    tile_grid.tile_size = scene.grid.tile_size;
    tile_grid.rows = scene.grid.rows;
    for (const auto& td : scene.grid.tiles) {
        TileGrid::Tile tile;
        tile.mesh = MeshManager::get().load_mesh(td.mesh);
        tile.rotation = td.rotation;
        tile_grid.tiles[td.key] = tile;
    }
    engine.set_tile_grid(tile_grid);

    // Shadow camera
    engine.shadow_camera().set_position(scene.shadow.position);
    engine.shadow_camera().set_direction(scene.shadow.direction);

    // Terrain
    if (!scene.terrain.source.empty()) {
        TerrainData::ShPtr terrain = TerrainLoader::load(scene.terrain.source);
        engine.set_terrain(terrain);
    }

    // Lighting
    engine.set_light_direction(scene.light.direction);
    engine.set_light_ambient(scene.light.ambient);
    engine.set_light_diffuse(scene.light.diffuse);
    engine.set_light_specular(scene.light.specular);
    engine.set_ambient_global(scene.light.ambient_global);

    return instances;
}
