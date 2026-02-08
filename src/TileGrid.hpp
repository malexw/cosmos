#ifndef COSMOS_TILEGRID_HPP_
#define COSMOS_TILEGRID_HPP_

#include <map>
#include <string>
#include <vector>

#include "Mesh.hpp"

struct TileGrid {
    struct Tile {
        Mesh::ShPtr mesh;
        float rotation; // degrees around Y
    };
    std::map<char, Tile> tiles;
    float tile_size = 4.0f;
    std::vector<std::string> rows;
};

#endif
