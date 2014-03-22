#include "Chunk.hpp"
#include "Tile.hpp"
#include "World.hpp"

Chunk::Chunk()
    : x(0), y(0), tileRows(5), tileCols(5), tileWidth(1), tileHeight(1)
{
}

Chunk::~Chunk()
{
    /*for(unsigned int i = 0 ; i < tiles.size() ; i++ )
    {
        delete tiles[i];
    }*/
}

void Chunk::load(World* _world, int _x, int _y, unsigned int _width, unsigned int _height)
{
    world = _world;
    x = _x;
    y = _y;
    tileWidth = _width;
    tileHeight = _height;

    const int tileArray[] =
    {
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 1,
    };
    int rows = 5;
    int cols = 5;

    tiles.resize(rows * cols, NULL);

    // Load Tiles
    for (unsigned int j = 0; j < rows; ++j)
    {
        for (unsigned int i = 0; i < cols; ++i)
        {
            // Index is is converted from a relative x and y position (i and j)
            int index = i + j * cols;

            // Tile position is the chunk position plus the relative position within the chunk
            int tileX = x + tileWidth * i;
            int tileY = y + tileHeight * j;

            // Create and load the tile, then hand it off to the world
            tiles[index] = new Tile();
            tiles[index]->load(world, world->tileIdCounter++, Tile::Type::Ground, tileX, tileY, tileWidth, tileHeight);
            world->tiles.push_back(tiles[index]);
        }
    }
}

Tile* Chunk::getTileAt(int _x, int _y)
{
    // Convert the x and y to tile coordinates offset
    int tx = _x - x;
    int ty = _y - y;

    // Convert to an index
    unsigned int index = tx / tileWidth + ty / tileHeight * tileCols;

    return tiles[index];
}