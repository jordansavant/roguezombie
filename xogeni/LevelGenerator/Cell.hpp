#pragma once
#ifndef XG_CELL_H
#define XG_CELL_H

namespace XoGeni
{
    class Cell
    {
    public:
        Cell();

        Cell(unsigned int index, unsigned int x, unsigned int y);

        unsigned int index;
        unsigned int x, y;

    };
}

#endif