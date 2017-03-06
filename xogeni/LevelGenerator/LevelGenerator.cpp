#include "LevelGenerator.hpp"
#include "../../bitengine/Math.hpp"
#include "CellMap.hpp"
#include "Entrance.hpp"
#include "Exit.hpp"

bit::Random XoGeni::LevelGenerator::random;

XoGeni::LevelGenerator::LevelGenerator()
{
}

XoGeni::LevelGenerator::~LevelGenerator()
{
}

std::vector<XoGeni::CellMap*> XoGeni::LevelGenerator::buildTower(unsigned int seed)
{
    random.seed(seed);

    unsigned int mapCount = 2;
    std::vector<CellMap*> maps;
    CellMap* parent = NULL;

    for(unsigned int mapId = 1; mapId <= mapCount; mapId++)
    {
        CellMap* map = generate(64, 64, mapId);
        
        // If we have a parent map then connect us
        if(parent != NULL)
        {
            parent->connectToChild(map);
            map->connectToParent(parent);
        }

        // If I am the last map then destroy my exit
        if(mapId == mapCount)
        {
            map->destroyExit();
        }

        parent = map;
        maps.push_back(map);
    }

    return maps;
}

XoGeni::CellMap* XoGeni::LevelGenerator::buildMap(unsigned int seed)
{
    random.seed(seed);

    return generate(64, 64, 1);

}

XoGeni::CellMap* XoGeni::LevelGenerator::generate(unsigned int width, unsigned int height, unsigned int mapId)
{
    CellMap* cellMap = new CellMap(mapId, width, height);

    cellMap->buildGround();
    cellMap->buildRooms();
    cellMap->buildTunnels();
    cellMap->buildDoors();
    cellMap->buildExits();
    cellMap->cleanupConnections();
    cellMap->buildWalls();
    cellMap->buildLights();

    return cellMap;
}
