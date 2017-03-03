#include "LevelGenerator.hpp"
#include "../../bitengine/Math.hpp"
#include "CellMap.hpp"

bit::Random XoGeni::LevelGenerator::random;

XoGeni::LevelGenerator::LevelGenerator()
{
}

XoGeni::LevelGenerator::~LevelGenerator()
{
}

XoGeni::CellMap* XoGeni::LevelGenerator::generate(unsigned int seed, unsigned int width, unsigned int height)
{
    random.seed(seed);

    CellMap* cellMap = new CellMap(width, height);

    cellMap->buildGround();
    cellMap->buildRooms();
    cellMap->buildDoors();

    return cellMap;
}
