#include "Room.hpp"
#include "RoomDoor.hpp"
#include "LevelGenerator.hpp"

#include <math.h> 

XoGeni::Room::Room()
    : x(0), y(0), width(0), height(0), entranceWeight(0), isMachineRoom(false)
{
}

XoGeni::Room::~Room()
{
}

XoGeni::Room::Room(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
    : x(x), y(y), width(width), height(height), entranceWeight(0)
{
}

unsigned int XoGeni::Room::cellCount()
{
    return width * height;
}
