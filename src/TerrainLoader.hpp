#ifndef COSMOS_TERRAINLOADER_HPP_
#define COSMOS_TERRAINLOADER_HPP_

#include <string>

#include "TerrainDefinition.hpp"
#include "TerrainData.hpp"

class TerrainLoader {
public:
    static TerrainDefinition load_definition(const std::string& path);
    static TerrainData::ShPtr instantiate(const TerrainDefinition& def);
    static TerrainData::ShPtr load(const std::string& path);
};

#endif
