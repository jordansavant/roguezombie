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
    unsigned int difficultyLevel = 0;

    for(unsigned int mapId = 1; mapId <= mapCount; mapId++)
    {
        CellMap* map = generate(64, 64, mapId, difficultyLevel);
        
        // If we have a parent map then connect us
        if(parent != NULL)
        {
            parent->connectToChild(map);
            map->connectToParent(parent);
        }

        parent = map;
        maps.push_back(map);

        difficultyLevel++;
    }

    return maps;
}

XoGeni::CellMap* XoGeni::LevelGenerator::buildMap(unsigned int seed, unsigned int difficultyLevel)
{
    random.seed(seed);

    CellMap* cellMap = generate(64, 64, 1, difficultyLevel);

    return cellMap;

}

XoGeni::CellMap* XoGeni::LevelGenerator::generate(unsigned int width, unsigned int height, unsigned int mapId, unsigned int difficultyLevel)
{
    CellMap* cellMap = new CellMap(mapId, width, height, difficultyLevel);

    cellMap->buildGround();
    cellMap->buildRooms();
    cellMap->buildTunnels();
    cellMap->buildDoors();
    cellMap->buildEntrance();
    cellMap->cleanupConnections();
    cellMap->calculateEntranceWeights();
    cellMap->buildExit();
    cellMap->buildWalls();
    cellMap->buildLights();
    cellMap->buildTags();

    cellMap->spawnEnemies();
    cellMap->spawnTreasure();
    // cellMap->spawnTraps();

    return cellMap;
}
