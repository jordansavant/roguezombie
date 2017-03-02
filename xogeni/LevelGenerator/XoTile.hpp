#pragma once
#ifndef XG_XOTILE_H
#define XG_XOTILE_H

namespace XoGeni
{
    class XoTile
    {
    public:
        XoTile();

        XoTile(unsigned int index, unsigned int x, unsigned int y);

        unsigned int index;
        unsigned int x, y;

    };
}

#endif