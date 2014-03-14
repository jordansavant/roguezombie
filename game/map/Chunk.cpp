#include "Chunk.hpp"
#include "Block.hpp"

Chunk::Chunk()
    : x(0), y(0), blockRows(0), blockCols(0), blockWidth(1), blockHeight(1)
{
}

void Chunk::load(int _x, int _y, unsigned int _width, unsigned int _height)
{
    x = _x;
    y = _y;
    blockWidth = _width;
    blockHeight = _height;

    const int tileArray[] =
    {
        1, 1, 1, 1, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 0, 0, 0, 1,
        1, 1, 1, 1, 1,
    };
    int rows = 5;
    int cols = 5;

    blocks.resize(rows * cols);
    for (unsigned int j = 0; j < rows; ++j)
    {
        for (unsigned int i = 0; i < cols; ++i)
        {
            int index = i + j * cols;
            int blockX = blockWidth * i;
            int blockY = blockHeight * j;

            blocks[index].load(tileArray[index], blockX, blockY);
        }
    }
}

Block& Chunk::getTileAt(int _x, int _y)
{
    unsigned int index = _x / blockWidth + _y / blockHeight * blockCols;

    return blocks[index];
}