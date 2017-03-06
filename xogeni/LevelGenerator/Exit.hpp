#pragma once
#ifndef XG_EXIT_H
#define XG_EXIT_H

namespace XoGeni
{
    class Exit
    {
    public:

        Exit(unsigned int id, unsigned int x, unsigned int y, unsigned int childMapId = 0, unsigned int childEntranceId = 0);

        unsigned int id;
        unsigned int x, y;
        bool isConnectedToChild;
        unsigned int childMapId;
        unsigned int childEntranceId;

    };
}

#endif