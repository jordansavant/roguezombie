#pragma once
#ifndef XG_ROOMDOOR_H
#define XG_ROOMDOOR_H

#include "../../bitengine/Math.hpp"

namespace XoGeni
{
    class Room;

    class RoomDoor
    {
    public:

        enum DoorType
        {
            Arch,
            Door,
        };

        RoomDoor(unsigned int x, unsigned int y, DoorType dt, Room* parentRoom, sf::Vector2i &direction);

        unsigned int x, y;
        DoorType doorType;
        Room* parentRoom;
        sf::Vector2i direction;

    };
}

#endif