#include "Cell.hpp"
#include "Room.hpp"

#include <stddef.h>

XoGeni::Cell::Cell(unsigned int index, unsigned int x, unsigned int y)
    : index(index), x(x), y(y), room(NULL), isWall(false), isRoomEdge(false), isRoomPermiter(false), isSill(false), isDoor(false), isTunnel(false)
{
}
