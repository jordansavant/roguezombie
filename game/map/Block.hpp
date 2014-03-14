#pragma once
#ifndef RZ_BLOCK_H
#define RZ_BLOCK_H

class Block
{
public:

    Block();

    int type;
    int x, y;

    void load(int type, int x, int y);
};

#endif