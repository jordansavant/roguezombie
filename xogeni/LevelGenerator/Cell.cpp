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
      isExitTransition(false), exitTransition(NULL),
      hasLight(false), lightRadius(0), lightOpacity(0), lightColorR(0), lightColorG(0), lightColorB(0),
      isTagUnreachable(false)
{
    aStarX = x;
    aStarY = y;
}
