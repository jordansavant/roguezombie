#pragma once
#ifndef XG_CELLMAP_H
#define XG_CELLMAP_H

#include <vector>
#include <functional>

namespace XoGeni
{
    class Cell;
    class Room;

    class CellMap
    {
    public:
        CellMap(unsigned int width, unsigned int height);

        ~CellMap();

        unsigned int width, height;
        unsigned int size;

        std::vector<XoGeni::Cell*> cells;
        std::vector<XoGeni::Room*> rooms;

        int roomCount;
        int roomAttemptCount;
        int mapPadding;
        int minRoomWidth;
        int minRoomHeight;

        void buildGround();

        void buildRooms();

        Room* buildRoom();

        void emplaceRoom(Room* room);

        bool canHouseDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h);

        void inspectAllCellsInSpiral(const std::function<bool(Cell* cell)> &inspector);

        void inspectCellsInDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::function<bool(Cell* cell)> &inspector);

        Cell* getCellAtPosition(unsigned int x, unsigned int y);
    };
}
#endif