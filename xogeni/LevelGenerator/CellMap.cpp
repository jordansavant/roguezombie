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

unsigned int ACCESS_LEVEL_YELLOW = 1;
unsigned int ACCESS_LEVEL_RED = 2;
unsigned int ACCESS_LEVEL_GREEN = 3;
unsigned int ACCESS_LEVEL_BLUE = 4;
unsigned int ACCESS_LEVEL_PURPLE = 5;

unsigned int CHARACTER_ZOMBIE = 1;
unsigned int CHARACTER_HUNTER = 2;
unsigned int CHARACTER_OGRE = 3;
unsigned int CHARACTER_SCIENTIST = 4;
unsigned int CHARACTER_GUARD = 5;

unsigned int STRUCTURE_WALL = 1;
unsigned int STRUCTURE_DOOR = 2;
unsigned int STRUCTURE_CHEST = 3;
unsigned int STRUCTURE_FURNISHING = 4;

unsigned int FURNISHING_TERMINALA = 1;
unsigned int FURNISHING_COLUMNA = 2;
unsigned int FURNISHING_TERMINALB = 3;
unsigned int FURNISHING_DESKA = 4;
unsigned int FURNISHING_DESKB = 5;
unsigned int FURNISHING_SINKA = 6;
unsigned int FURNISHING_SINKB = 7;
unsigned int FURNISHING_OPTABLEA = 8;
unsigned int FURNISHING_PILONA = 9;
unsigned int FURNISHING_TRASHCANA = 10;
unsigned int FURNISHING_TRASHCANB = 11;
unsigned int FURNISHING_ROLLINGTERMINALA = 12;
unsigned int FURNISHING_DESKBEAKERB = 13;
unsigned int FURNISHING_DESKCOFFEEA = 14;
unsigned int FURNISHING_SHELFA = 15;

unsigned int ITEM_BACKPACK = 1;
unsigned int ITEM_HARDHAT = 2;
unsigned int ITEM_MAGNUM357 = 3;
unsigned int ITEM_Z4RIFLE = 4;
unsigned int ITEM_CROWBAR = 5;
unsigned int ITEM_BATON = 6;
unsigned int ITEM_MEDKIT = 7;
unsigned int ITEM_BRICK = 8;
unsigned int ITEM_GRENADE = 9;
unsigned int ITEM_FOOTBALLPADS = 10;
unsigned int ITEM_COMBATBOOTS = 11;
unsigned int ITEM_RACINGPANTS = 12;
unsigned int ITEM_CLEANINGGLOVES = 13;
unsigned int ITEM_GOLDMEDAL = 14;
unsigned int ITEM_KEYCARD = 15;

unsigned int TRAP_SPIKE = 1;

XoGeni::CellMap::CellMap(unsigned int id, unsigned int width, unsigned int height, unsigned int difficultyLevel)
    : id(id), width(width), height(height), size(width * height), difficultyLevel(difficultyLevel),
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

unsigned int XoGeni::CellMap::difficultyMax = 4;

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
            // Officially a door so give it structure definition
            doorCell->hasStructure = true;
            doorCell->structureType = STRUCTURE_DOOR;
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
                            cell->isWall = true;
                            cell->hasStructure = true;
                            cell->structureType = STRUCTURE_WALL;
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
// ENVIRONMENT SPAWNING START
////////////////////////////////////////

void XoGeni::CellMap::machinate()
{
    // MASTER LEVEL BUILDING ROUTING
    // This routing uses the level's difficulty to build all machines, treasure, traps, furnishings, enemies, chests, keys etc


    // LEVEL 0
    // A level zero floor's goal is to introduce mechanics with a mild amount of challenge
    // It should mostly be populated with helpless enemies, simple chests, no traps and a single Level 1 enemy as a boss

    // MACHINES
    // Chest/Key/Treasure:
    // - Level 0 will reward a player with a pistol treasure
    // - It will also have treasures for less useful reasons, potentially plot oriented (drives?)

    machinate_chestKeyTreasure();

    machinate_boss();

    // designate machine rooms
    for (unsigned int i = 0; i < rooms.size(); i++)
    {
        Room* room = rooms[i];
        if (room == entranceRoom || room == exitRoom || room->width < 4 || room->height < 4)
        {
            room->isMachineRoom = false;
            nonMachineRooms.push_back(room);
        }
        else if (i % 2 == 0 && LevelGenerator::random.next(2) == 0)
        {
            room->isMachineRoom = true;
            machineRooms.push_back(room);
        }
        else
        {
            room->isMachineRoom = false;
            nonMachineRooms.push_back(room);
        }
    }

    // build machine rooms (advanced rooms potentially linked)
    for (unsigned int i = 0; i < machineRooms.size(); i++)
    {
        // TODO: Expand on more machine rooms
        machinate_trapRoom(machineRooms[i]);
    }

    // build non-machine rooms (standard with room flavors)
    for (unsigned int i = 0; i < nonMachineRooms.size(); i++)
    {
        Room* room = nonMachineRooms[i];

        decorateRoom(room);

        if (room != entranceRoom && room != exitRoom)
        {
            populateRoom(room);
        }
    }

}

void XoGeni::CellMap::machinate_trapRoom(Room* room)
{
    // Put a treasure chest in the middle of the room
    // TODO: Pick a trap type for the room

    Cell* centerCell = getRoomCenterCell(room);
    setChest(centerCell, false);
    giveChestTreasure(centerCell, false);
    unsigned int i = 0;

    // Puts traps in a uniform pattern with manageable gaps
    inspectRoomCells(room, [this, &i, room](Cell* cell) -> bool {
        unsigned int rx = cell->x - room->x;
        unsigned int ry = cell->y - room->y;
        if (i % 3 == 0 && rx % 3 != 0 && !cell->isOccupied() && this->isCellSafeToBlock(cell))
        {
            cell->isTrap = true;
            cell->trapType = TRAP_SPIKE;
        }
        i++;
        return false;
    });
}

void XoGeni::CellMap::machinate_chestKeyTreasure()
{
    // Pick a random room that is not the exit or entrance and spawn a chest that is locked
    // Generate a key for this chest
    // Generate a level X enemy in a room >= entranceDistance of chest not the exit/entrance and place the key on his body

    unsigned int accessLevel = getAccessLevelForDifficulty();
    unsigned int itemType = getWeaponForDifficulty();
    unsigned int standardEnemyType = getStandardEnemyTypeForDifficulty();

    // Set the chest
	Room* chestRoom = getRandomRoom(false, false);
    Cell* chestCell = getSafeToBlockRoomCell(chestRoom, true, 1);
    if (chestCell)
    {
        setChest(chestCell, true);
        giveChestTreasure(chestCell, true); // better quality items
        chestCell->structureAccessLevel = accessLevel;
        chestCell->inventory.push_back(Cell::ItemData(itemType)); // guarantee weapon-up reward
    }

    // Create enemy with keycard for chest
    Room* enemyRoom = getRandomRoom(false, false);
    Cell* enemyCell = getOpenRoomCell(enemyRoom, true);
    if (enemyCell)
    {
        enemyCell->hasCharacter = true;
        enemyCell->characterType = standardEnemyType;
        // give him a keycard
        enemyCell->inventory.push_back(Cell::ItemData(ITEM_KEYCARD, accessLevel));
        enemyCell->hasCharacterWithKey = true;
        enemyCell->characterKeyAccessLevel = ACCESS_LEVEL_YELLOW;
    }
}

void XoGeni::CellMap::machinate_boss()
{
    // Put a guard in the exit room
    Cell* cell = getOpenRoomCell(exitRoom, true);
    if (cell)
    {
        cell->hasCharacter = true;
        cell->characterType = getOutOfDepthEnemyTypeForDifficulty();
    }
}

void XoGeni::CellMap::setChest(Cell* cell, bool isLocked)
{
    cell->hasStructure = true;
    cell->decorate(STRUCTURE_CHEST); // chest
    cell->isLocked = isLocked;
}

void XoGeni::CellMap::giveChestTreasure(Cell* cell, bool isOutOfDepth)
{
    // Generate treasure for this chest cell (chest already set)

    // How many items?
    unsigned int count = LevelGenerator::random.next(1, 5);
    for (unsigned int i = 0; i < count; i++)
    {
        unsigned int treasureType = isOutOfDepth ? getOutOfDepthEnemyTypeForDifficulty() : getStandardTreasureItemTypeForDifficulty();
        cell->inventory.push_back(Cell::ItemData(treasureType));
    }
}

void XoGeni::CellMap::decorateRoom(Room* room)
{
    // Types of decor
    // - stuff that goes against walls, particularly North and East walls (for ISO display)
    // - patterns like columns in a room
    bool columnRoom = (LevelGenerator::random.next(2) == 0 && room->width > 8 && room->height > 8);
    if (columnRoom)
    {
        // Lets try and build out a column based room
        unsigned int wt = (room->width) / 3;
        unsigned int ht = (room->height) / 3;

        inspectRoomCells(room, [&wt, &ht, &room](Cell* cell) -> bool {

            // if this is a third from both edges
            if (cell->x == room->x + wt && cell->y == room->y + ht
                || cell->x == room->x + room->width - wt - 1 && cell->y == room->y + ht
                || cell->x == room->x + wt && cell->y == room->y + room->height - ht - 1
                || cell->x == room->x + room->width - wt - 1 && cell->y == room->y + room->height - ht - 1
            )
            {
                if (!cell->isTagUnreachable && !cell->isRoomEdge)
                {
                    cell->hasStructure = true;
                    cell->structureType = STRUCTURE_FURNISHING; // Furnishing
                    cell->structureSubType = FURNISHING_COLUMNA; // Column
                }
            }
            return false;
        });
    }

    // Random decor
    inspectRoomNorthCells(room, [this](Cell* cell) -> bool {
        if (!cell->isOccupied() && isCellSafeToBlock(cell)) {
            if (LevelGenerator::random.next(3) == 0)
            {
                cell->hasStructure = true;
                cell->decorate(STRUCTURE_FURNISHING, LevelGenerator::random.of(
                    FURNISHING_TERMINALA,
                    FURNISHING_DESKA,
                    FURNISHING_DESKA,
                    FURNISHING_SINKA,
                    FURNISHING_PILONA,
                    FURNISHING_TRASHCANA,
                    FURNISHING_TRASHCANB,
                    FURNISHING_ROLLINGTERMINALA,
                    FURNISHING_DESKCOFFEEA,
                    FURNISHING_SHELFA
                ));
            }
        }
        return false;
    });
    inspectRoomEastCells(room, [this](Cell* cell) -> bool {
        if (!cell->isOccupied() && isCellSafeToBlock(cell)) {
            if (LevelGenerator::random.next(3) == 0)
            {
                cell->hasStructure = true;
                cell->decorate(STRUCTURE_FURNISHING, LevelGenerator::random.of(
                    FURNISHING_TERMINALB,
                    FURNISHING_DESKB,
                    FURNISHING_DESKB,
                    FURNISHING_SINKB,
                    FURNISHING_PILONA,
                    FURNISHING_TRASHCANA,
                    FURNISHING_TRASHCANB,
                    FURNISHING_DESKBEAKERB
                ));
            }
        }
        return false;
    });
    inspectRoomSouthCells(room, [this](Cell* cell) -> bool {
        if (!cell->isOccupied() && isCellSafeToBlock(cell)) {
            if (LevelGenerator::random.next(6) == 0)
            {
                cell->hasStructure = true;
                cell->decorate(STRUCTURE_FURNISHING, LevelGenerator::random.of(
                    FURNISHING_DESKA,
                    FURNISHING_DESKA,
                    FURNISHING_DESKA,
                    FURNISHING_PILONA,
                    FURNISHING_TRASHCANA,
                    FURNISHING_TRASHCANB
                ));
            }
        }
        return false;
    });
    inspectRoomWestCells(room, [this](Cell* cell) -> bool {
        if (!cell->isOccupied() && isCellSafeToBlock(cell)) {
            if (LevelGenerator::random.next(6) == 0)
            {
                cell->hasStructure = true;
                cell->decorate(STRUCTURE_FURNISHING, LevelGenerator::random.of(
                    FURNISHING_DESKB,
                    FURNISHING_DESKB,
                    FURNISHING_DESKB,
                    FURNISHING_PILONA,
                    FURNISHING_TRASHCANA,
                    FURNISHING_TRASHCANB
                ));
            }
        }
        return false;
    });

    // Random stuff
    Cell * cell = getSafeToBlockRoomCell(room, true, 2);
    if (cell) {
        if (LevelGenerator::random.boolean()) {
            setChest(cell, false); // unlocked storage
            giveChestTreasure(cell, false);
        } else {
            cell->hasStructure = true;
            cell->decorate(STRUCTURE_FURNISHING, LevelGenerator::random.of(
                FURNISHING_OPTABLEA,
                FURNISHING_PILONA,
                FURNISHING_ROLLINGTERMINALA
            ));
        }
    }
}

void XoGeni::CellMap::populateRoom(Room* room)
{
    // Iterate rooms, place random enemy
    unsigned int spawnCount = room->entranceWeight / 2 + 1;

    for (unsigned int j = 0; j < spawnCount; j++)
    {
        // Test a random cell in the room until one is found
        Cell* cell = getOpenRoomCell(room, true);
        if (cell)
        {
            if (cell->hasStructure)
            {
                int ow = 12;
            }
            cell->hasCharacter = true;

            switch (difficultyLevel)
            {
                case 0:
                    cell->characterType = CHARACTER_SCIENTIST; // scientist
                    break;
                case 1:
                    cell->characterType = CHARACTER_GUARD; // guard
                    break;
                default:
                    cell->characterType = CHARACTER_HUNTER; // Hunter
                    break;
            }
        }
    }
}

unsigned int XoGeni::CellMap::getStandardEnemyTypeForDifficulty()
{
    switch (difficultyLevel)
    {
        case 0:
            return CHARACTER_SCIENTIST;
        case 1:
            return CHARACTER_GUARD;
        case 2:
            return CHARACTER_HUNTER;
        default:
            return CHARACTER_HUNTER; // TODO better enemy scaling
    }
}

unsigned int XoGeni::CellMap::getOutOfDepthEnemyTypeForDifficulty()
{
    switch (difficultyLevel)
    {
        case 0:
            return CHARACTER_GUARD;
        case 1:
            return CHARACTER_HUNTER;
        case 2:
            return CHARACTER_HUNTER;
        default:
            return CHARACTER_HUNTER; // TODO better enemy scaling
    }
}

unsigned int XoGeni::CellMap::getStandardTreasureItemTypeForDifficulty()
{
    std::vector<unsigned int> items;
    switch (difficultyLevel)
    {
        // NOTE THE CASCADE CASE!!! Each higher case adds items to lower cases
        default: // TODO: Increased depth scaling
        case 2:
            items.push_back(ITEM_GRENADE);
            items.push_back(ITEM_GOLDMEDAL);
            items.push_back(ITEM_GRENADE);
            items.push_back(ITEM_Z4RIFLE);
            items.push_back(ITEM_MEDKIT); // increased chance
        case 1:
            items.push_back(ITEM_CROWBAR);
            items.push_back(ITEM_HARDHAT);
            items.push_back(ITEM_MAGNUM357);
            items.push_back(ITEM_RACINGPANTS);
        case 0:
            items.push_back(ITEM_BRICK);
            items.push_back(ITEM_CLEANINGGLOVES);
            items.push_back(ITEM_BATON);
            items.push_back(ITEM_MEDKIT);
    }
    return items[LevelGenerator::random.next(items.size())];
}

unsigned int XoGeni::CellMap::getOutOfDepthTreasureItemTypeForDifficulty()
{
    std::vector<unsigned int> items;
    switch (difficultyLevel)
    {
        // DUPLICATE OF Standard BUT WITH CASES PUSHED DOWNWARD
        // NOTE THE CASCADE CASE!!! Each higher case adds items to lower cases
        default: // TODO: Increased depth scaling
        case 1:
            items.push_back(ITEM_GRENADE);
            items.push_back(ITEM_GOLDMEDAL);
            items.push_back(ITEM_GRENADE);
            items.push_back(ITEM_Z4RIFLE);
            items.push_back(ITEM_MEDKIT); // increased chance
        case 0:
            items.push_back(ITEM_CROWBAR);
            items.push_back(ITEM_HARDHAT);
            items.push_back(ITEM_MAGNUM357);
            items.push_back(ITEM_RACINGPANTS);
        //case 0:
            items.push_back(ITEM_BRICK);
            items.push_back(ITEM_CLEANINGGLOVES);
            items.push_back(ITEM_BATON);
            items.push_back(ITEM_MEDKIT);
    }
    return items[LevelGenerator::random.next(items.size())];
}

unsigned int XoGeni::CellMap::getAccessLevelForDifficulty()
{
    switch (difficultyLevel)
    {
        // TODO: Scale
        default:
            return ACCESS_LEVEL_YELLOW;
    }
}

unsigned int XoGeni::CellMap::getWeaponForDifficulty()
{
    switch (difficultyLevel)
    {
        // TODO: Scale, randomize
        default:
            return ITEM_MAGNUM357;
    }
}

/////////////////////////////////////////
// ENVIRONMENT SPAWNING END
////////////////////////////////////////





XoGeni::Room* XoGeni::CellMap::getRandomRoom(bool includeEntrance, bool includeExit)
{
	std::vector<Room*> availableRooms;
	for (unsigned int i = 0; i < rooms.size(); i++)
	{
		Room* room = rooms[i];
		if (room == entranceRoom)
		{
			if (includeEntrance)
                availableRooms.push_back(room);
		}
        else if (room == exitRoom)
        {
            if (includeExit)
                availableRooms.push_back(room);
        }
        else
        {
            availableRooms.push_back(room);
        }
	}
    return availableRooms[LevelGenerator::random.next(availableRooms.size())];
}

bool XoGeni::CellMap::isCellSafeToBlock(Cell* cell)
{
    Cell* northCell = getCellAtPosition(cell->x, cell->y - 1);
    Cell* eastCell = getCellAtPosition(cell->x - 1, cell->y);
    Cell* westCell = getCellAtPosition(cell->x + 1, cell->y);
    Cell* southCell = getCellAtPosition(cell->x, cell->y + 1);
    return (!northCell->isDoor && !eastCell->isDoor && !westCell->isDoor && !southCell->isDoor
        && !northCell->isTunnel && !eastCell->isTunnel && !westCell->isTunnel && !southCell->isTunnel);
}

XoGeni::Cell* XoGeni::CellMap::getRoomCenterCell(Room* room)
{
    unsigned int x = room->x + room->width / 2;
    unsigned int y = room->y + room->height / 2;

    return getCellAtPosition(x, y);
}


XoGeni::Cell* XoGeni::CellMap::getOpenRoomCell(Room* room, bool random)
{
    if (!random) {
        Cell* openCell = NULL;
        inspectCellsInDimension(room->x, room->y, room->width, room->height, [&openCell](Cell* cell) -> bool {
            if (!cell->isOccupied())
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
            if (!cell->isOccupied())
            {
                openCells.push_back(cell);
            }
            return false;
        });
        unsigned int r = LevelGenerator::random.next(openCells.size());
        return openCells[r];
    }
}


XoGeni::Cell* XoGeni::CellMap::getSafeToBlockRoomCell(Room* room, bool random, unsigned int padding = 0)
{
    if (!random) {
        Cell* openCell = NULL;
        inspectCellsInDimension(room->x + padding, room->y + padding, room->width - padding, room->height - padding, [this, &openCell](Cell* cell) -> bool {
            if (!cell->isOccupied() && isCellSafeToBlock(cell))
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
        inspectCellsInDimension(room->x + padding, room->y + padding, room->width - padding, room->height - padding, [this, &openCells](Cell* cell) -> bool {
            if (!cell->isOccupied() && isCellSafeToBlock(cell))
            {
                openCells.push_back(cell);
            }
            return false;
        });

        if (openCells.size() > 0)
        {
            unsigned int r = LevelGenerator::random.next(openCells.size());
            return openCells[r];
        }

        return NULL;
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

void XoGeni::CellMap::inspectRoomCells(Room* room, const std::function<bool(Cell* cell)> &inspector)
{
    inspectCellsInDimension(room->x, room->y, room->width, room->height, [&inspector] (Cell* cell) -> bool {
        return inspector(cell);
    });
}

void XoGeni::CellMap::inspectRoomNorthCells(Room* room, const std::function<bool(Cell* cell)> &inspector)
{
    for (unsigned int i = room->x; i < room->x + room->width; i++) // cols
    {
        Cell* cell = getCellAtPosition(i, room->y);
        bool complete = inspector(cell);
        if (complete)
            break;
    }
}

void XoGeni::CellMap::inspectRoomEastCells(Room* room, const std::function<bool(Cell* cell)> &inspector)
{
    for (unsigned int j = room->y; j < room->y + room->height; j++) // rows
    {
        Cell* cell = getCellAtPosition(room->x, j);
        bool complete = inspector(cell);
        if (complete)
            break;
    }
}

void XoGeni::CellMap::inspectRoomSouthCells(Room* room, const std::function<bool(Cell* cell)> &inspector)
{
    for (unsigned int i = room->x; i < room->x + room->width; i++) // cols
    {
        Cell* cell = getCellAtPosition(i, room->y + room->height - 1);
        bool complete = inspector(cell);
        if (complete)
            break;
    }
}

void XoGeni::CellMap::inspectRoomWestCells(Room* room, const std::function<bool(Cell* cell)> &inspector)
{
    for (unsigned int j = room->y; j < room->y + room->height; j++) // rows
    {
        Cell* cell = getCellAtPosition(room->x + room->width - 1, j);
        bool complete = inspector(cell);
        if (complete)
            break;
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