#pragma once
#ifndef XG_CELLMAP_H
#define XG_CELLMAP_H

#include <vector>
#include <functional>
#include "../../bitengine/Math.hpp"

namespace XoGeni
{
    class Cell;
    class Room;
    class RoomDoor;

    class CellMap
    {
    public:

        CellMap(unsigned int width, unsigned int height);

        ~CellMap();

        unsigned int width, height;
        unsigned int size;

        std::vector<XoGeni::Cell*> cells;
        std::vector<XoGeni::Room*> rooms;
        std::vector<XoGeni::RoomDoor*> doors;

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

        // Exit building

        void buildExits();

        // Clean up
        void cleanup();

        void collapseTunnels();

        void collapse(Cell* tunnelCell);

        unsigned int countTunnelConnections(Cell* tunnelCell);

        void fixDoors();

        void fixRooms();

        bool isRoomConnected(Room* room);

        bool areRoomsConnected(Room* room, Room* other);

        Room* getNearestRoom(Room* room);

        void tunnelFromRoomToRoom(Room* start, Room* end);

        void emplaceRoomFix(Cell* cell);


        bool canHouseDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

        void inspectAllCellsInSpiral(const std::function<bool(Cell* cell)> &inspector);

        void inspectCellsInDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::function<bool(Cell* cell)> &inspector);

        Cell* getCellAtPosition(unsigned int x, unsigned int y);

        Cell* getCellAtPositionNullable(unsigned int x, unsigned int y);

        void getShuffledDirections(std::vector<sf::Vector2i> &fill);


        // Test methods
        bool testValidity();

        bool areAllRoomsConnected();
    };
}
#endif