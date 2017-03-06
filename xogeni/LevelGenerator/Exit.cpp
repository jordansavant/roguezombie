#include "Exit.hpp"

XoGeni::Exit::Exit(unsigned int id, unsigned int x, unsigned int y, unsigned int childMapId, unsigned int childEntranceId)
    : id(id), x(x), y(y), childMapId(childMapId), childEntranceId(childEntranceId)
{
    isConnectedToChild = childMapId > 0;
}
