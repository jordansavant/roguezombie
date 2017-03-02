#pragma once
#ifndef XG_LEVELGENERATOR_H
#define XG_LEVELGENERATOR_H

namespace XoGeni
{
    class CellMap;

    class LevelGenerator
    {
    public:
        LevelGenerator();
        
        ~LevelGenerator();

        CellMap* generate(unsigned int seed, unsigned int width, unsigned int height);
    };
}
#endif