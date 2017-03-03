#pragma once
#ifndef XG_ROOMDOOR_H
#define XG_ROOMDOOR_H

namespace XoGeni
{
    class RoomDoor
    {
    public:

        enum DoorType
        {
            Arch,
            Door,
        };

        RoomDoor(unsigned int x, unsigned int y, DoorType dt);

        unsigned int x, y;
        DoorType doorType;

    };
}

#endif