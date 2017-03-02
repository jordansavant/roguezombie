#include "CellMap.hpp"
#include "Cell.hpp"

XoGeni::CellMap::CellMap(unsigned int width, unsigned int height)
    : width(width), height(height), size(width * height)
{
}

XoGeni::CellMap::~CellMap()
{
    for(unsigned int i=0; i < cells.size(); i++)
    {
        delete cells[i];
    }
}

void XoGeni::CellMap::buildGround()
{
    for(unsigned int i=0; i < size; i++)
    {
        unsigned int cellIndex = i;
        unsigned int cellX = i % width;
        unsigned int cellY = i / height;

        Cell* cell = new Cell(cellIndex, cellX, cellY);

        cells.push_back(cell);
    }
}

void XoGeni::CellMap::buildRooms()
{
}