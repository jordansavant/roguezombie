#include "LevelGenerator.hpp"
#include "../../bitengine/Math.hpp"
#include "CellMap.hpp"

XoGeni::LevelGenerator::LevelGenerator()
{
}

XoGeni::LevelGenerator::~LevelGenerator()
{
}

XoGeni::CellMap* XoGeni::LevelGenerator::generate(unsigned int seed, unsigned int width, unsigned int height)
{
    CellMap* cellMap = new CellMap(width, height);

    cellMap->buildGround();

    return cellMap;
}
