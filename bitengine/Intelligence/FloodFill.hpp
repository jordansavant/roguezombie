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

        static unsigned int floodfillId;

        static void compute(int x, int y, std::function<void(int, int, int)> inspect, std::function<bool(int, int, int)> isBlocked);

    private:        
        
        static void compute(int x, int y, std::function<void(int, int, int)> inspect, std::function<bool(int, int, int)> isBlocked, int depth);

    };
}

#endif