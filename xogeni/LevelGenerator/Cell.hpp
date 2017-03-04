#pragma once
#ifndef XG_CELL_H
#define XG_CELL_H

namespace XoGeni
{
    class Room;
    class RoomDoor;

    class Cell
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
        bool isWall;
        bool isRoomEdge;
        bool isRoomPermiter;
        bool isSill;
        bool isDoor;
        bool isTunnel;
        bool wasCorridorTunnel;
        bool wasDoorTunnel;

    };
}

#endif