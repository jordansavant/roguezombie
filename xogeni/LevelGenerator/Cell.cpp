#include "Cell.hpp"
#include "Room.hpp"

#include <stddef.h>

XoGeni::Cell::Cell(unsigned int index, unsigned int x, unsigned int y)
    : index(index), x(x), y(y), room(NULL), tunnelTestCount(0),
      isWall(false), isRoomEdge(false), isRoomPermiter(false), isSill(false),
      isDoor(false), wasDoor(false),
      isTunnel(false), wasCorridorTunnel(false), wasDoorTunnel(false),
      isEntrance(false), isExit(false), entranceId(0), exitId(0)
{
}
