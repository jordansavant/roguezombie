#pragma once
#ifndef XG_CELL_H
#define XG_CELL_H

namespace XoGeni
{
    class Room;

    class Cell
    {
    public:

        Cell(unsigned int index, unsigned int x, unsigned int y);

        unsigned int index;
        unsigned int x, y;
        Room* room;
        bool isWall;
        bool isRoomEdge;
        bool isSill;
        bool isDoor;

    };
}

#endif