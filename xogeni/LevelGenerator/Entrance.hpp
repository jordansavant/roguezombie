#pragma once
#ifndef XG_ENTRANCE_H
#define XG_ENTRANCE_H

namespace XoGeni
{
    class Entrance
    {
    public:

        Entrance(unsigned int id, unsigned int x, unsigned int y, unsigned int parentExitId = 0);

        unsigned int id;
        unsigned int x, y;
        bool isConnectedToParent;
        unsigned int parentExitId;

    };
}

#endif