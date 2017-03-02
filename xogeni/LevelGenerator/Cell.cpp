#include "Cell.hpp"

XoGeni::Cell::Cell()
    : index(0), x(0), y(0)
{
}

XoGeni::Cell::Cell(unsigned int index, unsigned int x, unsigned int y)
    : index(index), x(x), y(y)
{
}
