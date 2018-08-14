#include "CellMap.hpp"
#include "Cell.hpp"
#include "Room.hpp"
#include "RoomDoor.hpp"
#include "LevelGenerator.hpp"
#include "Entrance.hpp"
#include "Exit.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/Intelligence.hpp"
#include "../../bitengine/System.hpp"

XoGeni::CellMap::CellMap(unsigned int id, unsigned int width, unsigned int height)
    : id(id), width(width), height(height), size(width * height),
      entranceRoom(NULL), exitRoom(NULL)
{
    mapPadding = 1;
    float mapHypSize = std::sqrtf(width * height);

    // Room Details
    minRoomWidth = 6;
    maxRoomWidth = 16;
    minRoomHeight = 6;
    maxRoomHeight = 16;

    float hypMinRoomSize = std::sqrtf(minRoomWidth * minRoomHeight);
    float hypMaxRoomSize = std::sqrtf(maxRoomWidth * maxRoomHeight);
    float hypSize = (hypMinRoomSize + hypMaxRoomSize) / 2;

    float roomDensity = .8;
    float maxRoomsPerMap = (mapHypSize / hypSize) * (mapHypSize / hypSize);

    //roomDensity = .5f + (roomDensity / 2);
    roomCount = maxRoomsPerMap * roomDensity;
    roomAttemptCount = roomCount * 2;
    roomScatter = (hypSize * (1 - roomDensity)) + hypSize / 2;
    roomScatter = (hypSize / 2) + (hypSize * 10 * (1 - roomDensity)) ;

    // Tunnel details
    minHallWidth = 1;
    tunnelTurnRatio = 0;
    deadEndRatio = 0;

    tunnelDirs.push_back(sf::Vector2i(1, 0)); // right
    tunnelDirs.push_back(sf::Vector2i(0, 1)); // down
    tunnelDirs.push_back(sf::Vector2i(-1, 0)); // left
    tunnelDirs.push_back(sf::Vector2i(0, -1)); // up

    // Exit details
    entranceCount = 1;
    exitCount = 1;
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
    
    delete entrance;
    delete exit;
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
        doorSpot->wasDoor = true;
        doorSpot->door = door;

        // Help connections
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
                //tunnel2(cell);
                tunnel(cell);
            }
        }
    }
}

void XoGeni::CellMap::tunnel(Cell* cell, sf::Vector2i lastDir)
{
    // ORIGINAL
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
    }
    
    // Try to dig in any direction
    for(unsigned int i=0; i < dirs.size(); i++)
    {
        sf::Vector2i dir = dirs[i];
    
        if(attemptStraight && dir.x == lastDir.x && dir.y == lastDir.y)
            continue;
    
        if(openTunnel(cell, dir))
        {
            Cell* nextCell = getCellAtPosition(cell->x + dir.x, cell->y + dir.y);
            tunnel(nextCell, dir);
        }
    }
}

void XoGeni::CellMap::tunnel2(Cell* cell, unsigned int dirI)
{
    // If I have run this cell against all directions, fail
    if(cell->tunnelTestCount >= tunnelDirs.size())
        return;

    sf::Vector2i dir = tunnelDirs[dirI % tunnelDirs.size()];
    if(!openTunnel(cell, dir))
    {
        // If I cant tunnel in this dir, try again in next direction
        cell->tunnelTestCount++;
        return tunnel2(cell, dirI + 1);
    }

    // If I successfully tunneled get next cell and tunnel again
    Cell* nextCell = getCellAtPosition(cell->x + dir.x, cell->y + dir.y);
    return tunnel2(nextCell, dirI);
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
    markAsTunnel(nextCell);
}

void XoGeni::CellMap::markAsTunnel(Cell* cell)
{
    cell->isTunnel = true;
    cell->wasCorridorTunnel = true;
}

/////////////////////////////////////////
// TUNNEL BUILDING END
////////////////////////////////////////




/////////////////////////////////////////
// EXIT BUILDING START
////////////////////////////////////////

// Exit and entrances depend on connecting parent maps
// If there is no parent map we can create an entrance and exit
// If there is a parent we should tie the entrance to their exit and create an exit
// In either scenario we are the master of exits so generate them

// This is run before rooms are guaranteed connected because it is used as the primary connection point
void XoGeni::CellMap::buildEntrance()
{
    unsigned int entranceId = 1;

    // Pick a random room (in future give better heuristic)
    Room* room = rooms[LevelGenerator::random.next(rooms.size())];
    
    // Pick a good entrance cell
    Cell* entranceCell = pickTransitionCellForRoom(room);
    
    // Build entrance
    entrance = new Entrance(entranceId, entranceCell->x, entranceCell->y);
    entranceCell->isEntranceTransition = true;
    entranceCell->entranceTransition = entrance;

    // Build  landing pad
    buildLandingPad(entranceCell, entranceId);

    // Set our maps entrance room
    entranceRoom = room;
}

// This is run after rooms are connected so their weights to the entrance are calculated for exit placement
void XoGeni::CellMap::buildExit()
{
    unsigned int exitId = 2; // must not be same as entrance id

    // Pick a random room that is farthest from our entrance
    auto end = roomsByWeight.rbegin()->second;
    Room* room = end[LevelGenerator::random.next(end.size())];
    
    // Pick a good entrance cell
    Cell* exitCell = pickTransitionCellForRoom(room);
        
    // Build exit
    exit = new Exit(exitId, exitCell->x, exitCell->y);
    exitCell->isExitTransition = true;
    exitCell->exitTransition = exit;

    // Build  landing pad
    buildLandingPad(exitCell, exitId);

    // Set our maps exit room
    exitRoom = room;
}

XoGeni::Cell* XoGeni::CellMap::pickTransitionCellForRoom(XoGeni::Room* room)
{
    // Default to the center of the room because its guaranteed, though not amazing
    Cell* pick = NULL;
    unsigned int centerX = room->x + room->width / 2;
    unsigned int centerY = room->y + room->height / 2;
    pick = getCellAtPosition(centerX, centerY);

    // First pick is the northwest corner
    // It must have a wall to north and to west
    unsigned int nwX = room->x;
    unsigned int nwY = room->y;
    Cell* nwCell = getCellAtPosition(nwX, nwY);
    Cell* nwnCell = getCellAtPosition(nwX, nwY - 1);
    Cell* nwwCell = getCellAtPosition(nwX - 1, nwY);
    if(!nwnCell->isDoor && !nwnCell->isTunnel && !nwwCell->isDoor && !nwwCell->isTunnel)
    {
        pick = nwCell;
        pick->transitionDir.x = 0;
        pick->transitionDir.y = 1;
        return pick;
    }

    return pick;
}

void XoGeni::CellMap::buildLandingPad(Cell* startCell, unsigned int entranceId)
{
    // Build entrance landing pad
    unsigned int maxRadius = 3;
    CellMap* cellMap = this;
    bit::FloodFill::compute(startCell->x, startCell->y,
        [cellMap, &entranceId, startCell] (int x, int y, int depth) {
            Cell* cell = cellMap->getCellAtPosition(x, y);
            if(cell->metadata_floodfillId != bit::FloodFill::floodfillId)
            {
                // Entrance transition is not blocked so we can flodd from it outward, so dont assign it as a pad
                if(cell->isEntranceTransition == false && cell->isExitTransition == false)
                {
                    unsigned int radius = bit::VectorMath::distance(cell->x, cell->y, startCell->x, startCell->y);
                    cell->isEntrance = true;
                    cell->entranceId = entranceId;
                    cell->entrancePriority = radius;
                    cell->metadata_floodfillId = bit::FloodFill::floodfillId;
                }
            }
        },
        [cellMap, maxRadius, startCell] (int x, int y, int depth) -> bool {
            Cell* cell = cellMap->getCellAtPosition(x, y);
            unsigned int radius = bit::VectorMath::distance(cell->x, cell->y, startCell->x, startCell->y);
            return radius > maxRadius || cell->metadata_floodfillId == bit::FloodFill::floodfillId || cell->isRoomPermiter;
        }
    );
}


void XoGeni::CellMap::connectToParent(CellMap* parentMap)
{
    // Connect the entrance to the parent's exit
    entrance->isConnectedToParent = true;
    entrance->parentMapId = parentMap->id;
    entrance->parentExitId = parentMap->exit->id;
    Cell* entranceCell = getCellAtPosition(entrance->x, entrance->y);
    int a = 1;
}

void XoGeni::CellMap::connectToChild(CellMap* childMap)
{
    // Connect the exit to the child's entrance
    exit->isConnectedToChild = true;
    exit->childMapId = childMap->id;
    exit->childEntranceId = childMap->entrance->id;
    Cell* exitCell = getCellAtPosition(exit->x, exit->y);
    int a = 1;
}

void XoGeni::CellMap::calculateEntranceWeights()
{
    for(unsigned int i=0; i < rooms.size(); i++)
    {
        Room* room = rooms[i];
        unsigned int weight = 0;
        Room* lastRoom = NULL;
        if(room != entranceRoom)
        {
            // Get the path
            std::vector<Cell*> path;
            getRoomConnectionPath(room, entranceRoom, path);

            // Iterate the path and total the number of rooms in between
            for(unsigned int j=0; j < path.size(); j++)
            {
                Cell* cell = path[j];
                // If this inspected room is not our starting room or the entrance room increment our weight
                if(cell->room && cell->room != room && cell->room != entranceRoom && cell->room != lastRoom)
                {
                    lastRoom = cell->room;
                    weight++;
                }
            }
        }

        // Add to our list of rooms organized by weights
        auto itr = roomsByWeight.find(weight);
        if(itr == roomsByWeight.end())
        {
            roomsByWeight[weight];
        }

        // Add weight to room
        roomsByWeight[weight].push_back(room);
        room->entranceWeight = weight;
    }
}

/////////////////////////////////////////
// EXIT BUILDING END
////////////////////////////////////////





/////////////////////////////////////////
// CLEAN CONNECTIONS START
////////////////////////////////////////

void XoGeni::CellMap::cleanupConnections()
{
    // Remove dead ends
    collapseTunnels();

    // Fix doors
    fixDoors();

    // Fix rooms
    fixRooms();
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

void XoGeni::CellMap::fixDoors()
{
    //Door Cleanup
    //- Iterate all doors
    //    - If door direction has no tunnel, door or other room
    //        - delete door
    //  - If door direction is another door
    //      - delete door

    // Fix dangling doors
    for(auto it = doors.begin(); it != doors.end();)
    {
        RoomDoor* door = (*it);
        Cell* doorCell = getCellAtPosition(door->x, door->y);
        Cell* neighbor = getCellAtPosition(door->x + door->direction.x, door->y + door->direction.y);

        // See if we need to remove the element
        if((neighbor->room == NULL && !neighbor->isDoor && !neighbor->isTunnel))
        {
            doorCell->isDoor = false;
            delete door;
            it = doors.erase(it);
        }
        else
        {
            ++it; // iterate list pointer
        }
    }

    // Fix doubled doors
    for(auto it = doors.begin(); it != doors.end();)
    {
        RoomDoor* door = (*it);
        Cell* doorCell = getCellAtPosition(door->x, door->y);
        Cell* neighbor = getCellAtPosition(door->x + door->direction.x, door->y + door->direction.y);

        if(neighbor->isDoor)
        {
            doorCell->isDoor = false;
            markAsTunnel(doorCell);
            delete door;
            it = doors.erase(it);
        }
        else
        {
            ++it; // iterate list pointer
        }
    }
}

// Finds rooms that cannot path to entrance and connects them with tunnels
// Counts weights of distance from entrance room, valued by the number of other rooms traversed to reach them
void XoGeni::CellMap::fixRooms()
{
    //- If more than one room
    //    - Iterate rooms
    //        - Iterate all doors connected to room
    //            - Floodfill tunnel until a room is located
    //                - If room is not origin room
    //                    - Increment origin room connections counter
    //        - If connections is 0
    //            - OPTION A: delete room
    //            - OPTION B: find closest room and tunnel toward room

    // Iterate rooms
    // Astar from their center to the center of every room
    // Count every other room connected
    // If any room has no connections it must be fixed
    if(rooms.size() > 1)
    {
        for(unsigned int i=0; i < rooms.size(); i++)
        {
            Room* room = rooms[i];
            if(!areRoomsConnected(room, entranceRoom))
            {
                // Tunnel to room
                tunnelFromRoomToRoom(room, entranceRoom, true);
            }
        }
    }
}

bool XoGeni::CellMap::isRoomConnected(Room* room)
{
    return areRoomsConnected(room, entranceRoom);
}

bool XoGeni::CellMap::areRoomsConnected(Room* room, Room* otherRoom)
{
    if(room == otherRoom)
    {
        return true;
    }

    std::vector<Cell*> cellPath;
    getRoomConnectionPath(room, otherRoom, cellPath);

    if(cellPath.size() != 0)
    {
        return true;
    }

    return false;
}

void XoGeni::CellMap::getRoomConnectionPath(Room* room, Room* otherRoom, std::vector<Cell*> &fill)
{
    CellMap* cellMap = this;
    unsigned int centerX = room->x + room->width / 2;
    unsigned int centerY = room->y + room->height / 2;
    Cell* roomCenterCell = getCellAtPosition(centerX, centerY);

    if(otherRoom == room)
        return;

    unsigned int otherCenterX = otherRoom->x + otherRoom->width / 2;
    unsigned int otherCenterY = otherRoom->y + otherRoom->height / 2;
    Cell* otherRoomCenterCell = getCellAtPosition(otherCenterX, otherCenterY);

    std::function<bool(Cell* c)> isBlocked = [&room] (Cell* c) -> bool {
        if(c->room == NULL && !c->isTunnel && !c->isDoor)
        {
            return true;
        }
        return false;
    };
    std::function<void(Cell* c, std::vector<Cell*> &f)> getNeighbors = [&cellMap] (Cell* c, std::vector<Cell*> &f) {
        f.push_back(cellMap->getCellAtPosition(c->x, c->y - 1)); // top
        f.push_back(cellMap->getCellAtPosition(c->x, c->y + 1)); // bottom
        f.push_back(cellMap->getCellAtPosition(c->x + 1, c->y)); // right
        f.push_back(cellMap->getCellAtPosition(c->x - 1, c->y)); // left
    };

    bit::Astar::pathfind(roomCenterCell, otherRoomCenterCell, isBlocked, getNeighbors, fill);
}

XoGeni::Room* XoGeni::CellMap::getNearestRoom(Room* room)
{
    unsigned int centerX = room->x + room->width / 2;
    unsigned int centerY = room->y + room->height / 2;
    Cell* roomCenterCell = getCellAtPosition(centerX, centerY);
    Room* closestRoom = NULL;
    float closestDistance = 0;

    for(unsigned int i=0; i < rooms.size(); i++)
    {
        Room* otherRoom = rooms[i];
        if(otherRoom == room)
            continue;
        unsigned int otherCenterX = otherRoom->x + otherRoom->width / 2;
        unsigned int otherCenterY = otherRoom->y + otherRoom->height / 2;
        Cell* otherRoomCenterCell = getCellAtPosition(otherCenterX, otherCenterY);

        float distance = bit::VectorMath::distance(roomCenterCell->x, roomCenterCell->y, otherRoomCenterCell->x, otherRoomCenterCell->y);

        if(closestRoom == NULL || closestDistance > distance)
        {
            closestRoom = otherRoom;
            closestDistance = distance;
        }
    }

    return closestRoom;
}

void XoGeni::CellMap::tunnelFromRoomToRoom(Room* start, Room* end, bool stopOnRoom)
{
    Cell* startCenterCell = getCellAtPosition(start->x + start->width / 2, start->y + start->height / 2);
    Cell* endCenterCell = getCellAtPosition(end->x + end->width / 2, end->y + end->height / 2);

    sf::Vector2f dirf = bit::VectorMath::directionToVector(startCenterCell->x, startCenterCell->y, endCenterCell->x, endCenterCell->y);
    float distance = bit::VectorMath::distance(startCenterCell->x, startCenterCell->y, endCenterCell->x, endCenterCell->y);

    sf::Vector2i dir(bit::Math::round(dirf.x * distance), bit::Math::round(dirf.y * distance));
    sf::Vector2i pos(startCenterCell->x, startCenterCell->y);

    std::vector<sf::Vector2i> path;

    // Full orthogonal tunnel
    for(int i=1; i <= std::abs(dir.x); i++)
    {
        int xPlus = dir.x < 0 ? -1 : 1;
        pos.x += xPlus;
        path.push_back(pos);
    }
    for(int i=1; i <= std::abs(dir.y); i++)
    {
        int yPlus = dir.y < 0 ? -1 : 1;
        pos.y += yPlus;
        path.push_back(pos);
    }

    // Tunnel through path until another room is reached
    for(unsigned int i=0; i < path.size(); i++)
    {
        Cell* cell = getCellAtPosition(path[i].x, path[i].y);

        // If this is supposed to stop when we hit any room
        if(stopOnRoom)
        {
            // Check the local cell to see if we should stop
            if(cell->room && cell->room != start)
                return;

            // Check to see if we are adjacent to a room
            std::vector<Cell*> cardinalCells;
            getCardinalCells(cell->x, cell->y, cardinalCells);
            for(unsigned int j=0; j < cardinalCells.size(); j++)
            {
                Cell* cardinalCell = cardinalCells[j];
                if(cardinalCell->room && cardinalCell->room != start)
                {
                    // Dig this cell and return
                    emplaceRoomFix(cell);
                    return;
                }
            }
        }

        if(cell->room && cell->room != start && stopOnRoom)
            return;

        emplaceRoomFix(cell);
    }
}

void XoGeni::CellMap::emplaceRoomFix(Cell* cell)
{
    // Make door
    //if(cell->isRoomPermiter && !cell->isDoor && !cell->isTunnel)
    //{
    //    cell->isDoor = true;
    //}
    //// Make tunnel
    //else
    if(cell->room == NULL && !cell->isDoor && !cell->isTunnel)
    {
        cell->isTunnel = true;
        cell->wasRoomFixTunnel = true;
    }
}

/////////////////////////////////////////
// CLEAN UP CONNECTIONS END
////////////////////////////////////////






/////////////////////////////////////////
// WALL BUILDING START
////////////////////////////////////////

void XoGeni::CellMap::buildWalls()
{
    std::vector<sf::Vector2i> dirs;
    // cardinals
    dirs.push_back(sf::Vector2i(1, 0));
    dirs.push_back(sf::Vector2i(0, 1));
    dirs.push_back(sf::Vector2i(-1, 0));
    dirs.push_back(sf::Vector2i(0, -1));
    // diagnoals
    dirs.push_back(sf::Vector2i(1, 1));
    dirs.push_back(sf::Vector2i(-1, 1));
    dirs.push_back(sf::Vector2i(-1, -1));
    dirs.push_back(sf::Vector2i(1, -1));

    // Iterate all cells
    // If I am not a room, door or tunnel
    // Look at neighbors and count number of: doors, rooms, tunnels
    // If count is > 0 make me a wall
    for(unsigned int i = 0; i < width ; i++) // cols
    {
        for(unsigned int j = 0; j < height; j++) // rows
        {
            Cell* cell = getCellAtPosition(i, j);

            if(cell->room == NULL && !cell->isDoor && !cell->isTunnel)
            {
                for(unsigned int d = 0; d < dirs.size() ; d++)
                {
                    Cell* neighbor = getCellAtPositionNullable(cell->x + dirs[d].x, cell->y + dirs[d].y);
                    if(neighbor)
                    {
                        if(neighbor->room != NULL || neighbor->isDoor || neighbor->isTunnel)
                        {
                            cell->isWall = 1;
                        }
                    }
                }
            }
        }
    }
}

/////////////////////////////////////////
// WALL BUILDING END
////////////////////////////////////////






/////////////////////////////////////////
// LIGHT BUILDING START
////////////////////////////////////////

void XoGeni::CellMap::buildLights()
{
    // Lights are very costly

    // Add a light to the entrance
    Cell* entranceCell = getCellAtPosition(entrance->x, entrance->y);
    
    entranceCell->hasLight = true;
    entranceCell->lightRadius = 4;
    entranceCell->lightOpacity = .6;
    entranceCell->lightColorR = 200;
    entranceCell->lightColorG = 70;
    entranceCell->lightColorB = 0;

    // Add a light to the exit
    Cell* exitCell = getCellAtPosition(exit->x, exit->y);
    
    exitCell->hasLight = true;
    exitCell->lightRadius = 4;
    exitCell->lightOpacity = .6;
    exitCell->lightColorR = 200;
    exitCell->lightColorG = 70;
    exitCell->lightColorB = 0;
}

/////////////////////////////////////////
// LIGHT BUILDING END
////////////////////////////////////////





/////////////////////////////////////////
// TAG BUILDING START
////////////////////////////////////////

void XoGeni::CellMap::buildTags()
{
    // Identify cells that are unreachable
    tagUnreachableCells();
}

void XoGeni::CellMap::tagUnreachableCells()
{
    for(unsigned int i = 0; i < width ; i++) // cols
    {
        for(unsigned int j = 0; j < height; j++) // rows
        {
            Cell* cell = getCellAtPosition(i, j);

            if(cell->room == NULL && !cell->isDoor && !cell->isTunnel && !cell->isWall)
            {
                cell->isTagUnreachable = true;
            }
        }
    }
}

/////////////////////////////////////////
// TAG BUILDING END
////////////////////////////////////////




/////////////////////////////////////////
// ENEMY SPAWNING START
////////////////////////////////////////

void XoGeni::CellMap::spawnEnemies()
{
    // Iterate rooms, place random enemy
    for (unsigned int i = 0; i < rooms.size(); i++)
    {
        // Test a random cell in the room until one is found
        Cell* cell = getOpenRoomCell(rooms[i], true);
        if (cell) {
            cell->hasEnemy = true;
            cell->enemyType = 1;
        }
    }
}

/////////////////////////////////////////
// ENEMY SPAWNING END
////////////////////////////////////////



XoGeni::Cell* XoGeni::CellMap::getOpenRoomCell(Room* room, bool random)
{
    if (!random) {
        Cell* openCell = NULL;
        inspectCellsInDimension(room->x, room->y, room->width, room->height, [&openCell](Cell* cell) -> bool {
            if (!cell->isTagUnreachable)
            {
                openCell = cell;
                return true; // break inspection loop
            }
            return false;
        });
        return openCell;
    }
    else {
        unsigned int cellCount = room->width * room->height;
        std::vector<Cell*> openCells;
        inspectCellsInDimension(room->x, room->y, room->width, room->height, [&openCells](Cell* cell) -> bool {
            if (!cell->isTagUnreachable)
            {
                openCells.push_back(cell);
            }
            return false;
        });
        unsigned int r = LevelGenerator::random.next(openCells.size());
        return openCells[r];
    }
}



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

bool XoGeni::CellMap::testValidity()
{
    return areAllRoomsConnected();
}

bool XoGeni::CellMap::areAllRoomsConnected()
{
    for(unsigned int i=0; i < rooms.size(); i++)
    {
        if(!isRoomConnected(rooms[i]))
        {
            return false;
        }
    }

    return true;
}


void XoGeni::CellMap::getCardinalCells(unsigned int x, unsigned int y, std::vector<Cell*> &fill)
{
    Cell* north = getCellAtPositionNullable(x, y - 1);
    Cell* east = getCellAtPositionNullable(x + 1, y);
    Cell* south = getCellAtPositionNullable(x, y + 1);
    Cell* west = getCellAtPositionNullable(x - 1, y);

    if(north)
        fill.push_back(north);
    if(east)
        fill.push_back(east);
    if(south)
        fill.push_back(south);
    if(west)
        fill.push_back(west);
}