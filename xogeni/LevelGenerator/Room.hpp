#pragma once
#ifndef XG_ROOM_H
#define XG_ROOM_H

namespace XoGeni
{
    class Room
    {
    public:
        Room();

        Room(unsigned int x, unsigned int y, unsigned int width, unsigned int height);

        unsigned int x, y;
        unsigned int width, height;

        unsigned int randOpeningCount();

    };
}

#endif