#pragma once
#ifndef XG_XOTILEMAP_H
#define XG_XOTILEMAP_H

#include <vector>

namespace XoGeni
{
    class XoTile;

    class XoTileMap
    {
    public:
        XoTileMap(unsigned int width, unsigned int height);

        ~XoTileMap();

        unsigned int width, height;
        unsigned int size;

        std::vector<XoGeni::XoTile*> tiles;

        void buildGround();

        void buildRooms();
    };
}
#endif