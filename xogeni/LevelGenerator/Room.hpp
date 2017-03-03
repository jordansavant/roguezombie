#pragma once
#ifndef XG_ROOM_H
#define XG_ROOM_H

#include <vector>

namespace XoGeni
{
    class RoomDoor;

    class Room
    {
    public:
        Room();

        ~Room();

        Room(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

        unsigned int x, y;
        unsigned int width, height;
        std::vector<RoomDoor*> doors;

    };
}

#endif