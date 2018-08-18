#pragma once
#ifndef XG_LEVELGENERATOR_H
#define XG_LEVELGENERATOR_H

#include "../../bitengine/Math.hpp"

namespace XoGeni
{
    class CellMap;

    class LevelGenerator
    {
    public:
        LevelGenerator();
        
        ~LevelGenerator();

        static bit::Random random;
        
        std::vector<CellMap*> buildTower(unsigned int seed);
        
        CellMap* buildMap(unsigned int seed, unsigned int difficultyLevel);
        
        CellMap* generate(unsigned int width, unsigned int height, unsigned int mapId, unsigned int difficultyLevel);
    };
}
#endif