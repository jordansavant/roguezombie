#include "Cell.hpp"
#include "Room.hpp"

#include <stddef.h>

XoGeni::Cell::Cell(unsigned int index, unsigned int x, unsigned int y)
    : index(index), x(x), y(y), room(NULL), tunnelTestCount(0),
    isWall(false), isRoomEdge(false), isRoomPermiter(false), isSill(false),
    isDoor(false), wasDoor(false),
    isTunnel(false), wasCorridorTunnel(false), wasDoorTunnel(false), wasRoomFixTunnel(false),
    isEntrance(false), entranceId(0), entrancePriority(0),
    isEntranceTransition(false), entranceTransition(NULL),
    isExitTransition(false), exitTransition(NULL), transitionDir(0, 0),
    hasLight(false), lightRadius(0), lightOpacity(0), lightColorR(0), lightColorG(0), lightColorB(0),
    isTagUnreachable(false),
    hasCharacter(false), characterType(0),
    hasStructure(false), structureType(0), structureSubType(0),
    metadata_floodfillId(0)
{
    aStarX = x;
    aStarY = y;
}

bool XoGeni::Cell::isOccupied()
{
    return isTagUnreachable || isWall || isDoor || hasCharacter || hasStructure || isEntrance || isEntranceTransition || isExitTransition;
}
