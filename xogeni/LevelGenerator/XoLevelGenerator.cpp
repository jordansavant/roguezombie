#include "XoLevelGenerator.hpp"
#include "../../bitengine/Math.hpp"
#include "XoTileMap.hpp"

XoGeni::XoLevelGenerator::XoLevelGenerator()
{
}

XoGeni::XoLevelGenerator::~XoLevelGenerator()
{
}

XoGeni::XoTileMap* XoGeni::XoLevelGenerator::generate(unsigned int seed, unsigned int width, unsigned int height)
{
    XoTileMap* tileMap = new XoTileMap(width, height);

    return tileMap;
}
