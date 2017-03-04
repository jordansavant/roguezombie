#include "CellMap.hpp"
#include "Cell.hpp"
#include "Room.hpp"
#include "RoomDoor.hpp"
#include "LevelGenerator.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/Intelligence.hpp"

XoGeni::CellMap::CellMap(unsigned int width, unsigned int height)
    : width(width), height(height), size(width * height)
{
    roomCount = (width + height) / 4;
    roomAttemptCount = roomCount * 2;
    mapPadding = 1;
    minRoomWidth = 6;
    maxRoomWidth = 16;
    minRoomHeight = 6;
    maxRoomHeight = 16;
    roomScatter = 10;
    minHallWidth = 1;
    tunnelTurnRatio = 0;
    deadEndRatio = 0;

    tunnelDirs.push_back(sf::Vector2i(1, 0)); // right
    tunnelDirs.push_back(sf::Vector2i(0, 1)); // down
    tunnelDirs.push_back(sf::Vector2i(-1, 0)); // left
    tunnelDirs.push_back(sf::Vector2i(0, -1)); // up
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
    
    for(unsigned int i=0; i < doors.size(); i++)
    {
        delete doors[i];
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


/////////////////////////////////////////
// ROOM BUILDING START
////////////////////////////////////////

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
    unsigned int roomWidth = LevelGenerator::random.next(minRoomWidth, maxRoomWidth);
    unsigned int roomHeight = LevelGenerator::random.next(minRoomHeight, maxRoomHeight);

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
                    if(cell->room != NULL || cell->isRoomPermiter)
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
    CellMap* cellMap = this;
    inspectCellsInDimension(room->x, room->y, room->width, room->height, [room, cellMap] (Cell* cell) -> bool {
        cell->room = room;

        // Set room edge
        cell->isRoomEdge = (cell->x == room->x || cell->y == room->y || cell->x == room->x + room->width - 1 || cell->y == room->y + room->height - 1);

        // Set exterior walls
        if(cell->isRoomEdge)
        {
            // Top left corner
            if(cell->x == room->x && cell->y == room->y)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x - 1, cell->y - 1);
                neighbor->isRoomPermiter = true;
            }
            // Top Right corner
            if(cell->x == room->x + room->width - 1 && cell->y == room->y)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x + 1, cell->y - 1);
                neighbor->isRoomPermiter = true;
            }
            // Bottom left corner
            if(cell->x == room->x && cell->y == room->y + room->height - 1)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x - 1, cell->y + 1);
                neighbor->isRoomPermiter = true;
            }
            // Bottom right corner
            if(cell->x == room->x + room->width - 1 && cell->y == room->y + room->height - 1)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x + 1, cell->y + 1);
                neighbor->isRoomPermiter = true;
            }

            // Left side
            if(cell->x == room->x)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x - 1, cell->y);
                neighbor->isRoomPermiter = true;
            }
            // Top side
            if(cell->y == room->y)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x, cell->y - 1);
                neighbor->isRoomPermiter = true;
            }
            // Right side
            if(cell->x == room->x + room->width - 1)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x + 1, cell->y);
                neighbor->isRoomPermiter = true;
            }
            // Bottom side
            if(cell->y == room->y + room->height - 1)
            {
                Cell* neighbor = cellMap->getCellAtPosition(cell->x, cell->y + 1);
                neighbor->isRoomPermiter = true;
            }
        }

        return false;
    });
}

/////////////////////////////////////////
// ROOM BUILDING END
////////////////////////////////////////




/////////////////////////////////////////
// DOOR BUILDING START
////////////////////////////////////////

void XoGeni::CellMap::buildDoors()
{
    for(unsigned int i = 0; i < rooms.size(); i++)
    {
        openRoom(rooms[i]);
    }
}

void XoGeni::CellMap::openRoom(Room* room)
{
    // Get list of room door sills
    std::vector<Cell*> sills;
    std::vector<Cell*> doorSills;
    getRoomSills(room, sills);

    // Calculate number of openings
    unsigned int hypSize = std::sqrtf(room->width * room->height);
    unsigned int numOpenings = hypSize + LevelGenerator::random.next(hypSize);

    // Guarantee one doorway
    unsigned int firstDoorIndex = LevelGenerator::random.next(sills.size());
    doorSills.push_back(sills[firstDoorIndex]);

    // Get other random doors
    for(unsigned int i=0; i < sills.size(); i++)
    {
        if(LevelGenerator::random.next(numOpenings / 2) == 0 && i != firstDoorIndex)
        {
            doorSills.push_back(sills[i]);
        }
    }

    // Make doors
    for(unsigned int i=0; i < doorSills.size(); i++)
    {
        Cell* sill = doorSills[i];
        Cell* doorSpot = getCellAtPosition(sill->sillData.doorX, sill->sillData.doorY);

        if(doorSpot->isDoor)
            continue;

        // Pick door type
        RoomDoor::DoorType doorType;
        switch(LevelGenerator::random.next(2))
        {
            case 0:
                doorType = RoomDoor::DoorType::Arch;
                break;
            case 1:
                doorType = RoomDoor::DoorType::Door;
                break;
        }

        // Create door
        sf::Vector2i dir(doorSpot->x - sill->x, doorSpot->y - sill->y);
        RoomDoor* door = new RoomDoor(doorSpot->x, doorSpot->y, doorType, doorSills[i]->room, dir);
        doors.push_back(door);

        // Assign door
        doorSpot->isDoor = true;
        doorSpot->door = door;

        // Ensure connection
        connectDoor(door);
    }
}

void XoGeni::CellMap::getRoomSills(Room* room, std::vector<Cell*> &fill)
{
    unsigned int topMargin = minHallWidth + 2;
    unsigned int bottomMargin = height - (minHallWidth + 2);
    unsigned int leftMargin = minHallWidth + 2;
    unsigned int rightMargin = width - (minHallWidth + 2);
    unsigned int cornerSpacing = 2; // do not let sills be valid within 2 spaces of corners

    // North wall
    if(room->y > topMargin)
    {
        for(unsigned int i = room->x + cornerSpacing; i < room->x + room->width - cornerSpacing; i++)
        {
            if(i % 2 == 0)
            {
                Cell* cell = getCellAtPosition(i, room->y);
                cell->isSill = true;
                cell->sillData.doorX = cell->x;
                cell->sillData.doorY = cell->y - 1;
                fill.push_back(cell);
            }
        }
    }

    // South wall
    if(room->y + room->height < bottomMargin)
    {
        for(unsigned int i = room->x + cornerSpacing; i < room->x + room->width - cornerSpacing; i++)
        {
            if(i % 2 == 0)
            {
                Cell* cell = getCellAtPosition(i, room->y + room->height - 1);
                cell->isSill = true;
                cell->sillData.doorX = cell->x;
                cell->sillData.doorY = cell->y + 1;
                fill.push_back(cell);
            }
        }
    }

    // East wall
    if(room->x > leftMargin)
    {
        for(unsigned int i = room->y + cornerSpacing; i < room->y + room->height - cornerSpacing; i++)
        {
            if(i % 2 == 0)
            {
                Cell* cell = getCellAtPosition(room->x, i);
                cell->isSill = true;
                cell->sillData.doorX = cell->x - 1;
                cell->sillData.doorY = cell->y;
                fill.push_back(cell);
            }
        }
    }

    // West wall
    if(room->x + room->width < rightMargin)
    {
        for(unsigned int i = room->y + cornerSpacing; i < room->y + room->height - cornerSpacing; i++)
        {
            if(i % 2 == 0)
            {
                Cell* cell = getCellAtPosition(room->x + room->width - 1, i);
                cell->isSill = true;
                cell->sillData.doorX = cell->x + 1;
                cell->sillData.doorY = cell->y;
                fill.push_back(cell);
            }
        }
    }
}

void XoGeni::CellMap::connectDoor(RoomDoor* door)
{
    // Tunnel away from door until we find
    // - A tunnel
    // - Another door
    // - Another room

    bool complete = false;
    Cell* cell = getCellAtPosition(door->x, door->y);
    while(!complete)
    {
        Cell* rightCell = getCellAtPositionNullable(cell->x - door->direction.y, cell->y + door->direction.x);
        Cell* leftCell = getCellAtPositionNullable(cell->x + door->direction.y, cell->y - door->direction.x);
        cell = getCellAtPositionNullable(cell->x + door->direction.x, cell->y + door->direction.y);
        if(cell)
        {
            bool stop = cell->isTunnel || cell->isDoor || cell->isRoomEdge || // next cell
                        rightCell->isTunnel || leftCell->isTunnel || // side is a tunnel
                        rightCell->isRoomEdge || leftCell->isRoomEdge || // side is a room
                        rightCell->isDoor || leftCell->isDoor; // side is a door
            if(stop)
            {
                complete = true;
            }
            else
            {
                cell->isTunnel = true; 
                cell->wasDoorTunnel = true;
            }
        }
        else
        {
            // TODO: Failure condition
            complete = true;
        }
    }
}

/////////////////////////////////////////
// DOOR BUILDING END
////////////////////////////////////////





/////////////////////////////////////////
// TUNNEL BUILDING START
////////////////////////////////////////

void XoGeni::CellMap::buildTunnels()
{
    // Iterate all cells within the map
    for(unsigned int i = mapPadding; i < width - mapPadding; i++) // cols
    {
        for(unsigned int j = mapPadding; j < height - mapPadding; j++) // rows
        {
            // Build recursive tunnels from cell
            Cell* cell = getCellAtPosition(i, j);
            if(cell->room == NULL && !cell->isRoomPermiter && !cell->isTunnel)
            {
                tunnel(cell);
            }
        }
    }
}

void XoGeni::CellMap::tunnel(Cell* cell, sf::Vector2i lastDir)
{
    if(lastDir.x == 0 && lastDir.y == 0)
    {
        // first cell
        lastDir = tunnelDirs[0];
    }

    bool attemptStraight = LevelGenerator::random.nextFloat() > tunnelTurnRatio; // % chance to try to go straight
    std::vector<sf::Vector2i> dirs;
    getShuffledDirections(dirs);

    // Try to dig straight
    if(attemptStraight)
    {
        if(openTunnel(cell, lastDir))
        {
            Cell* nextCell = getCellAtPosition(cell->x + lastDir.x, cell->y + lastDir.y);
            tunnel(nextCell, lastDir);
        }

        // Remove this direction from choice directions
        unsigned int lastDirIndex;
        for(unsigned int i=0; i < dirs.size(); i++)
        {
            if(dirs[i].x == lastDir.x && dirs[i].y == lastDir.y)
            {
                lastDirIndex = i;
            }
        }
        dirs.erase(dirs.begin() + lastDirIndex);
    }

    // Try to dig in any direction
    for(unsigned int i=0; i < dirs.size(); i++)
    {
        sf::Vector2i dir = dirs[i];

        if(openTunnel(cell, dir))
        {
            Cell* nextCell = getCellAtPosition(cell->x + dir.x, cell->y + dir.y);
            tunnel(nextCell, dir);
        }
    }
}

bool XoGeni::CellMap::openTunnel(Cell* cell, sf::Vector2i &dir)
{
    if(canTunnel(cell, dir))
    {
        emplaceTunnel(cell, dir);

        return true;
    }

    return false;
}

bool XoGeni::CellMap::canTunnel(Cell* cell, sf::Vector2i &dir)
{
    unsigned int thisX = cell->x;
    unsigned int thisY = cell->y;

    unsigned int nextX = cell->x + dir.x;
    unsigned int nextY = cell->y + dir.y;
    unsigned int nextRightX = nextX + -dir.y;
    unsigned int nextRightY = nextY + dir.x;
    unsigned int nextLeftX = nextX + dir.y;
    unsigned int nextLeftY = nextY + -dir.x;

    unsigned int nextUpX = nextX + dir.x;
    unsigned int nextUpY = nextY + dir.y;
    unsigned int nextUpRightX = nextUpX + -dir.y;
    unsigned int nextUpRightY = nextUpY + dir.x;
    unsigned int nextUpLeftX = nextUpX + dir.y;
    unsigned int nextUpLeftY = nextUpY + -dir.x;

    // Cell two positions over cannot be: outside of margin, room perimeter, another corridor
    if(nextX >= mapPadding && nextY >= mapPadding && nextX < width - mapPadding && nextY < height - mapPadding)
    {
        Cell* nextCell = getCellAtPosition(nextX, nextY);
        Cell* nextLeftCell = getCellAtPosition(nextLeftX, nextLeftY);
        Cell* nextRightCell = getCellAtPosition(nextRightX, nextRightY);
        Cell* nextUpCell = getCellAtPosition(nextUpX, nextUpY);
        Cell* nextUpLeftCell = getCellAtPosition(nextUpLeftX, nextUpLeftY);
        Cell* nextUpRightCell = getCellAtPosition(nextUpRightX, nextUpRightY);
        
        bool isHeadingIntoRoom = nextCell->isRoomPermiter || nextUpCell->isRoomPermiter;
        bool isHeadingIntoTunnel = nextCell->isTunnel || nextUpCell->isTunnel;
        bool isRunningAdjacentToTunnel = nextLeftCell->isTunnel || nextRightCell->isTunnel || nextUpLeftCell->isTunnel || nextUpRightCell->isTunnel;

        if(!isHeadingIntoRoom && !isHeadingIntoTunnel && !isRunningAdjacentToTunnel)
        {
            return true;
        }
    }

    return false;
}

void XoGeni::CellMap::emplaceTunnel(Cell* cell, sf::Vector2i &dir)
{
    Cell* nextCell = getCellAtPosition(cell->x + dir.x, cell->y + dir.y);
    nextCell->isTunnel = true;
    nextCell->wasCorridorTunnel = true;
}

/////////////////////////////////////////
// TUNNEL BUILDING END
////////////////////////////////////////




/////////////////////////////////////////
// CLEAN UP START
////////////////////////////////////////

void XoGeni::CellMap::cleanup()
{
    // Remove dead ends
    collapseTunnels();
    // if tunnel and connected tunnels < 2 its a dead end
    // if dead end recursively collapse

    // Fix doors
    // Option A: attempt to redig tunnels
    // Option B: remove unconnected doors and ensure connectivity of rooms elsewhere
    // Doors stacked against each other from adjacent rooms should turn one into a tunnel (or arch?)

    // Empty blocks
}

void XoGeni::CellMap::collapseTunnels()
{
    // scan all cells
    for(unsigned int i = mapPadding; i < width - mapPadding; i++) // cols
    {
        for(unsigned int j = mapPadding; j < height - mapPadding; j++) // rows
        {
            Cell* cell = getCellAtPosition(i, j);
            // if tunnel and connected tunnels < 2 its a dead end
            // if dead end recursively collapse
            // Build recursive tunnels from cell
            bool p = LevelGenerator::random.nextFloat() > deadEndRatio;
            if(p && cell->isTunnel && countTunnelConnections(cell) < 2)
            {
                collapse(cell);
            }
        }
    }
}

void XoGeni::CellMap::collapse(Cell* tunnelCell)
{
    // collapse this tunnel
    tunnelCell->isTunnel = false;

    // Find direction of adjacent tunnel(s) // should be one unless used outside of collapsing dead ends
    for(unsigned int i=0; i < tunnelDirs.size(); i++)
    {
        // Collapse them if they are dead ends
        Cell* neighbor = getCellAtPositionNullable(tunnelCell->x + tunnelDirs[i].x, tunnelCell->y + tunnelDirs[i].y);
        if(neighbor && neighbor->isTunnel && countTunnelConnections(neighbor) < 2)
        {
            collapse(neighbor);
        }
    }
}

unsigned int XoGeni::CellMap::countTunnelConnections(Cell* tunnelCell)
{
    unsigned int count = 0;

    // Look at cardinal tiles
    Cell* topCell = getCellAtPositionNullable(tunnelCell->x, tunnelCell->y - 1);
    Cell* bottomCell = getCellAtPositionNullable(tunnelCell->x, tunnelCell->y + 1);
    Cell* rightCell = getCellAtPositionNullable(tunnelCell->x + 1, tunnelCell->y);
    Cell* leftCell = getCellAtPositionNullable(tunnelCell->x - 1, tunnelCell->y);

    count += topCell != NULL && (topCell->isTunnel || topCell->isDoor || topCell->isRoomEdge) ? 1 : 0;
    count += bottomCell != NULL && (bottomCell->isTunnel || bottomCell->isDoor || bottomCell->isRoomEdge) ? 1 : 0;
    count += rightCell != NULL && (rightCell->isTunnel || rightCell->isDoor || rightCell->isRoomEdge) ? 1 : 0;
    count += leftCell != NULL && (leftCell->isTunnel || leftCell->isDoor || leftCell->isRoomEdge) ? 1 : 0;

    return count;
}

/////////////////////////////////////////
// CLEAN UP END
////////////////////////////////////////



bool XoGeni::CellMap::canHouseDimension(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
{
    if(x > mapPadding && y > mapPadding)
    {
        if(x + w + mapPadding < width && y + h + mapPadding < height)
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

XoGeni::Cell* XoGeni::CellMap::getCellAtPositionNullable(unsigned int x, unsigned int y)
{
    if(x > 0 && y > 0 && x < width && y < height)
    {
        unsigned int index = x + (height * y);

        return cells[index];
    }

    return NULL;
}

void XoGeni::CellMap::getShuffledDirections(std::vector<sf::Vector2i> &fill)
{
    fill = tunnelDirs;

    unsigned int n = fill.size();
    for (unsigned int i = n - 1; i > 0; --i)
    {
        std::swap(fill[i], fill[LevelGenerator::random.next(i+1)]);
    }
}