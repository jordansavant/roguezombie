#include "RoomDoor.hpp"

XoGeni::RoomDoor::RoomDoor(unsigned int x, unsigned int y, DoorType dt, Room* parentRoom, sf::Vector2i &direction)
    : x(x), y(y), doorType(dt), parentRoom(parentRoom), direction(direction)
{
}
