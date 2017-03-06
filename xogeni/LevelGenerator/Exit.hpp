#pragma once
#ifndef XG_EXIT_H
#define XG_EXIT_H

namespace XoGeni
{
    class Exit
    {
    public:

        Exit(unsigned int id, unsigned int x, unsigned int y);

        unsigned int id;
        unsigned int x, y;

    };
}

#endif