#pragma once
#ifndef BIT_SPIRAL_H
#define BIT_SPIRAL_H

namespace bit
{
    class Spiral
    {
    public:

        Spiral();

        int x, y;

        void next();

    protected:

        unsigned int layer;
        unsigned int leg;
    };
}

#endif