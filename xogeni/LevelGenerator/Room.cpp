#include "Room.hpp"
#include "RoomDoor.hpp"
#include "LevelGenerator.hpp"

#include <math.h> 

XoGeni::Room::Room()
    : x(0), y(0), width(0), height(0)
{
}

XoGeni::Room::~Room()
{
}

XoGeni::Room::Room(unsigned int x, unsigned int y, unsigned int width, unsigned int height)
    : x(x), y(y), width(width), height(height)
{
}

