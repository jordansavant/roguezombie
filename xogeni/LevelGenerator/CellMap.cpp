#include "CellMap.hpp"
#include "Cell.hpp"
#include "Room.hpp"

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
    
    for(unsigned int i=0; i < rooms.size(); i++)
    {
        delete rooms[i];
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
    int roomCount = 10;
    int attemptCount = 1000;

    for(unsigned int a = 0; a < attemptCount; a++)
    {
        Room* room;
        if(a == 0)
        {
            // First room
            room = new Room(1, 1, 10, 10);
            emplaceRoom(room);
            rooms.push_back(room);
        }
        else
        {
            // 
        }

        if(rooms.size() > roomCount)
        {
            return;
        }
    }
}

void XoGeni::CellMap::emplaceRoom(Room* room)
{
    // Assign cells to have this room
    inspectCellsInDimension(room->x, room->y, room->width, room->height, [room] (Cell* cell) {
        cell->room = room;
        cell->isRoomEdge = (cell->x == room->x || cell->y == room->y || cell->x == room->x + room->width - 1 || cell->y == room->y + room->height - 1);
    });
}

void XoGeni::CellMap::inspectCellsInDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::function<void(Cell* cell)> &inspector)
{
    for(unsigned int i = x; i < x + w; i++) // cols
    {
        for(unsigned int j = y; j < y + h; j++) // rows
        {
            Cell* cell = getCellAtPosition(i, j);
            inspector(cell);
        }
    }
}

XoGeni::Cell* XoGeni::CellMap::getCellAtPosition(unsigned int x, unsigned int y)
{
    unsigned int index = x + (height * y);

    return cells[index];
}
