#include "CellMap.hpp"
#include "Cell.hpp"
#include "Room.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/Intelligence.hpp"

XoGeni::CellMap::CellMap(unsigned int width, unsigned int height)
    : width(width), height(height), size(width * height)
{
    roomCount = 50;
    roomAttemptCount = 100;
    mapPadding = 1;
    minRoomWidth = 6;
    maxRoomWidth = 16;
    minRoomHeight = 6;
    maxRoomHeight = 16;
    roomScatter = 5;
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
    for(unsigned int a = 0; a < roomAttemptCount; a++)
    {
        Room* room = buildRoom();
        if(room)
        {
            rooms.push_back(room);
        }

        if(rooms.size() > roomCount)
        {
            return;
        }
    }
}

XoGeni::Room* XoGeni::CellMap::buildRoom()
{
    unsigned int roomWidth = bit::Math::random(minRoomWidth, maxRoomWidth);
    unsigned int roomHeight = bit::Math::random(minRoomHeight, maxRoomHeight);

    Room* room = NULL;
    CellMap* cellMap = this;
    unsigned int i=0;

    inspectAllCellsInSpiral([roomWidth, roomHeight, cellMap, &room, &i] (Cell* cell) -> bool {

        if(i % cellMap->roomScatter == 0)
        {
            // Distribute according to room scatter distance
            if(cellMap->canHouseDimension(cell->x, cell->y, roomWidth, roomHeight))
            {
                // See if cells at this position within the room dimension are free
                bool canBePlaced = true;
                cellMap->inspectCellsInDimension(cell->x, cell->y, roomWidth, roomHeight, [&canBePlaced] (Cell* cell) -> bool {
                    if(cell->room != NULL)
                    {
                        canBePlaced = false;
                        return true; // break inspection loop
                    }
                    return false;
                });

                // If the cells are free then emplace the room
                if(canBePlaced)
                {
                    room = new Room(cell->x, cell->y, roomWidth, roomHeight);
                    cellMap->emplaceRoom(room);
                    return true;
                }
            }
        }

        i++;

        return false;
    });

    return room;
}

void XoGeni::CellMap::emplaceRoom(Room* room)
{
    // Assign cells to have this room
    inspectCellsInDimension(room->x, room->y, room->width, room->height, [room] (Cell* cell) -> bool {
        cell->room = room;
        cell->isRoomEdge = (cell->x == room->x || cell->y == room->y || cell->x == room->x + room->width - 1 || cell->y == room->y + room->height - 1);
        return false;
    });
}

bool XoGeni::CellMap::canHouseDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    if(x > 0 && y > 0)
    {
        if(x + w < width && y + h < height)
        {
            return true;
        }
    }

    return false;
}

void XoGeni::CellMap::inspectCellsInDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h, const std::function<bool(Cell* cell)> &inspector)
{
    for(unsigned int i = x; i < x + w; i++) // cols
    {
        for(unsigned int j = y; j < y + h; j++) // rows
        {
            Cell* cell = getCellAtPosition(i, j);
            bool complete = inspector(cell);
            if(complete)
            {
                break;
            }
        }
    }
}

void XoGeni::CellMap::inspectAllCellsInSpiral(const std::function<bool(Cell* cell)> &inspector)
{
    bit::Spiral spiral;

    // Starting point
    unsigned int centerX = width / 2 - 1;
    unsigned int centerY = height / 2 - 1;

    // Iterate cells
    for(unsigned int i=0; i < cells.size(); i++)
    {
        unsigned int currentX = centerX + spiral.x;
        unsigned int currentY = centerY + spiral.y;

        Cell* current = getCellAtPosition(currentX, currentY);
        bool complete = inspector(current);

        spiral.next();

        if(complete)
        {
            return;
        }
    }
}

XoGeni::Cell* XoGeni::CellMap::getCellAtPosition(unsigned int x, unsigned int y)
{
    unsigned int index = x + (height * y);

    return cells[index];
}
