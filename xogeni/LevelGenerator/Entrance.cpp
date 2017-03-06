#include "Entrance.hpp"

XoGeni::Entrance::Entrance(unsigned int id, unsigned int x, unsigned int y, unsigned int parentMapId, unsigned int parentExitId)
    : id(id), x(x), y(y), parentMapId(parentMapId), parentExitId(parentExitId)
{
    isConnectedToParent = parentMapId > 0;
}
