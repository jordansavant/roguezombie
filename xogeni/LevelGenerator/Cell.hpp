#pragma once
#ifndef XG_CELL_H
#define XG_CELL_H

#include "../../bitengine/Intelligence.hpp"

namespace XoGeni
{
    class Room;
    class RoomDoor;

    class Cell : public bit::Node
    {
    public:

        Cell(unsigned int index, unsigned int x, unsigned int y);

        struct SillData
        {
            SillData(): doorX(0), doorY(0) {}
            unsigned int doorX;
            unsigned int doorY;
        };

        unsigned int index;
        unsigned int x, y;
        Room* room;
        RoomDoor* door;
        SillData sillData;
        unsigned int tunnelTestCount;
        bool isWall;
        bool isRoomEdge;
        bool isRoomPermiter;
        bool isSill;
        bool isDoor;
        bool wasDoor;
        bool isTunnel;
        bool wasCorridorTunnel;
        bool wasDoorTunnel;
        bool wasRoomFixTunnel;
        bool isEntrance;
        bool isExit;
        unsigned int entranceId;
        unsigned int exitId;
        unsigned int metadata_floodfillId;

    };
}

#endif