#pragma once
#ifndef BIT_FLOODFILL_H
#define BIT_FLOODFILL_H

#include <functional>
#include <vector>

namespace bit
{
    class FloodFill
    {
    public:

        static void compute(unsigned int x, unsigned int y, std::function<void(int, int)> inspect, std::function<bool(int, int)> isBlocked);
        
    };
}

#endif