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

std::vector<XoGeni::CellMap*> XoGeni::LevelGenerator::buildTower(unsigned int seed)
{
    random.seed(seed);

    unsigned int mapCount = 5;
    std::vector<CellMap*> maps;
    CellMap* parent = NULL;

    for(unsigned int mapId = 1; mapId <= mapCount; mapId++)
    {
        CellMap* map;
        if(parent == NULL)
        {
            map = generate(seed, 64, 64, mapId);
        }
        else
        {
            map = generate(seed, 64, 64, mapId, parent);
        }
        parent = map;
        maps.push_back(map);
    }

    return maps;
}

XoGeni::CellMap* XoGeni::LevelGenerator::generate(unsigned int seed, unsigned int width, unsigned int height, unsigned int mapId, CellMap* parentMap)
{
    CellMap* cellMap = new CellMap(mapId, width, height, parentMap);

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
