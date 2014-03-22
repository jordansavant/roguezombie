#pragma once
#ifndef RZ_CHUNK_H
#define RZ_CHUNK_H

#include "Tile.hpp"
#include <vector>

class World;

class Chunk
{
public:

    Chunk();

    ~Chunk();

    World* world;
    int x, y;
    unsigned int tileRows, tileCols;
    unsigned int tileWidth, tileHeight;
    std::vector<Tile*> tiles;

    void load(World* world, int x, int y, unsigned int width, unsigned int height);

    Tile* getTileAt(int x, int y);

};

#endif