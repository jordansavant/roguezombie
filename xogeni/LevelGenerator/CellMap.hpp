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

        void buildGround();

        void buildRooms();

        void emplaceRoom(Room* room);

        void inspectCellsInDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::function<void(Cell* cell)> &inspector);

        Cell* getCellAtPosition(unsigned int x, unsigned int y);
    };
}
#endif