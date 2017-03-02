#pragma once
#ifndef XG_XOLEVELGENERATOR_H
#define XG_XOLEVELGENERATOR_H

namespace XoGeni
{
    class XoTileMap;

    class XoLevelGenerator
    {
    public:
        XoLevelGenerator();
        
        ~XoLevelGenerator();

        XoTileMap* generate(unsigned int seed, unsigned int width, unsigned int height);
    };
}
#endif