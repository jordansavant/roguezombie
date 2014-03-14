#pragma once
#ifndef RZ_CHUNK_H
#define RZ_CHUNK_H

#include "Block.hpp"
#include <vector>

class Chunk
{
public:

    Chunk();

    int x, y;
    unsigned int blockRows, blockCols;
    unsigned int blockWidth, blockHeight;
    std::vector<Block> blocks;

    void load(int x, int y, unsigned int width, unsigned int height);

    Block& getTileAt(int x, int y);

};

#endif