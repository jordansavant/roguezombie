#include "Entrance.hpp"

#include <stddef.h>

XoGeni::Entrance::Entrance(unsigned int id, unsigned int x, unsigned int y, unsigned int parentExitId)
    : id(id), x(x), y(y), parentExitId(parentExitId)
{
    isConnectedToParent = parentExitId > 0;
}
