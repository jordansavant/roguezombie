#pragma once
#ifndef XG_CELLMAP_H
#define XG_CELLMAP_H

#include <vector>

namespace XoGeni
{
    class Cell;

    class CellMap
    {
    public:
        CellMap(unsigned int width, unsigned int height);

        ~CellMap();

        unsigned int width, height;
        unsigned int size;

        std::vector<XoGeni::Cell*> cells;

        void buildGround();

        void buildRooms();
    };
}
#endif