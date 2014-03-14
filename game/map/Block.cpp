#include "Block.hpp"

Block::Block()
    : type(0), x(0), y(0)
{
}

void Block::load(int _type, int _x, int _y)
{
    type = _type;
    x = _x;
    y = _y;
}