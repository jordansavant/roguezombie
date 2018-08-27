#pragma once
#ifndef XG_CELLMAP_H
#define XG_CELLMAP_H

#include <vector>
#include <map>
#include <functional>
#include "../../bitengine/Math.hpp"

namespace XoGeni
{
    class Cell;
    class Room;
    class RoomDoor;
    class Entrance;
    class Exit;

    class CellMap
    {
    public:

        CellMap(unsigned int id, unsigned int width, unsigned int height, unsigned int difficultyLevel);

        ~CellMap();

        unsigned int id;
        unsigned int width, height;
        unsigned int size;
        unsigned int difficultyLevel;
        static unsigned int difficultyMax;

        std::vector<XoGeni::Cell*> cells;
        std::vector<XoGeni::Room*> rooms;
        std::vector<XoGeni::RoomDoor*> doors;
        XoGeni::Entrance* entrance;
        XoGeni::Exit* exit;
        std::map<unsigned int, std::vector<XoGeni::Room*>> roomsByWeight;

        int roomCount;
        int roomAttemptCount;
        int mapPadding;
        int minRoomWidth, maxRoomWidth;
        int minRoomHeight, maxRoomHeight;
        int roomScatter;
        int minHallWidth;
        float tunnelTurnRatio;
        float deadEndRatio;
        std::vector<sf::Vector2i> tunnelDirs;
        unsigned int entranceCount;
        unsigned int exitCount;
        Room* entranceRoom;
        Room* exitRoom;


        // Terrain building
        void buildGround();


        // Room building
        void buildRooms();

        Room* buildRoom();

        void emplaceRoom(Room* room);


        // Door building
        void buildDoors();

        void openRoom(Room* room);

        void getRoomSills(Room* room, std::vector<Cell*> &fill);

        void getRandomDoorType();

        void connectDoor(RoomDoor* door);


        // Tunnel building
        void buildTunnels();

        void tunnel(Cell* cell, sf::Vector2i lastDir = sf::Vector2i(0,0));

        void tunnel2(Cell* cell, unsigned int dirI = 0);

        bool openTunnel(Cell* cell, sf::Vector2i &dir);

        bool canTunnel(Cell* cell, sf::Vector2i &dir);

        void emplaceTunnel(Cell* cell, sf::Vector2i &dir);

        void markAsTunnel(Cell* cell);


        // Exit building
        void buildEntrance();

        void buildExit();

        Cell* pickTransitionCellForRoom(Room* room);

        void buildLandingPad(Cell* startCell, unsigned int entranceId);

        void connectToParent(CellMap* parentMap);

        void connectToChild(CellMap* child);

        void calculateEntranceWeights();


        // Clean up connections
        void cleanupConnections();

        void collapseTunnels();

        void collapse(Cell* tunnelCell);

        unsigned int countTunnelConnections(Cell* tunnelCell);

        void fixDoors();

        void fixRooms();

        bool isRoomConnected(Room* room);

        bool areRoomsConnected(Room* room, Room* other);

        void getRoomConnectionPath(Room* start, Room* end, std::vector<Cell*> &fill);

        Room* getNearestRoom(Room* room);

        void tunnelFromRoomToRoom(Room* start, Room* end, bool stopOnRoom = false);

        void emplaceRoomFix(Cell* cell);


        // Wall building

        void buildWalls();


        // Light building

        void buildLights();


        // Tag building

        void buildTags();

        void tagUnreachableCells();


        // Environment spawning

        void machinate();

        void machinate_chestKeyTreasure();

        void machinate_boss();

        void setChest(Cell* cell, bool isLocked);

        void spawnDecor();

        void spawnEnemies();

        unsigned int getStandardEnemyTypeForDifficulty();

        unsigned int getOutOfDepthEnemyTypeForDifficulty();

        unsigned int getAccessLevelForDifficulty();

        unsigned int getWeaponForDifficulty();

        //void spawnTreasure();


		Room* getRandomRoom(bool includeEntrance, bool includeExit);

        bool isCellSafeToBlock(Cell* cell);

        Cell* getOpenRoomCell(Room* room, bool random);

        Cell* getSafeToBlockRoomCell(Room* room, bool random, unsigned int padding);

        bool canHouseDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

        void inspectAllCellsInSpiral(const std::function<bool(Cell* cell)> &inspector);

        void inspectCellsInDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::function<bool(Cell* cell)> &inspector);

        void inspectRoomCells(Room* room, const std::function<bool(Cell* cell)> &inspector);

        void inspectRoomNorthCells(Room* room, const std::function<bool(Cell* cell)> &inspector);

        void inspectRoomEastCells(Room* room, const std::function<bool(Cell* cell)> &inspector);

        void inspectRoomSouthCells(Room* room, const std::function<bool(Cell* cell)> &inspector);

        void inspectRoomWestCells(Room* room, const std::function<bool(Cell* cell)> &inspector);

        Cell* getCellAtPosition(unsigned int x, unsigned int y);

        Cell* getCellAtPositionNullable(unsigned int x, unsigned int y);

        void getShuffledDirections(std::vector<sf::Vector2i> &fill);

        void getCardinalCells(unsigned int x, unsigned int y, std::vector<Cell*> &fill);


        // Test methods
        bool testValidity();

        bool areAllRoomsConnected();
    };
}
#endif