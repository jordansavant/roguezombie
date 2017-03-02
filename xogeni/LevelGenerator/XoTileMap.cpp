#include "XoTileMap.hpp"
#include "XoTile.hpp"

XoGeni::XoTileMap::XoTileMap(unsigned int width, unsigned int height)
    : width(width), height(height), size(width * height)
{
}

XoGeni::XoTileMap::~XoTileMap()
{
    for(unsigned int i=0; i < tiles.size(); i++)
    {
        delete tiles[i];
    }
}

void XoGeni::XoTileMap::buildGround()
{
    for(unsigned int i=0; i < size; i++)
    {
        unsigned int tileIndex = i;
        unsigned int tileX = i % width;
        unsigned int tileY = i / height;

        XoTile* tile = new XoTile(tileIndex, tileX, tileY);

        tiles.push_back(tile);
    }
}

void XoGeni::XoTileMap::buildRooms()
{
}