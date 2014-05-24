#include "FloodFill.hpp"

unsigned int bit::FloodFill::floodfillId = 0;

void bit::FloodFill::compute(int x, int y, std::function<void(int, int, int)> inspect, std::function<bool(int, int, int)> isBlocked)
{
    floodfillId++;
    compute(x, y, inspect, isBlocked, 0);
}

void bit::FloodFill::compute(int x, int y, std::function<void(int, int, int)> inspect, std::function<bool(int, int, int)> isBlocked, int depth)
{
    if(isBlocked(x, y, depth))
        return;
    
    inspect(x, y, depth);

    depth++;
    
    compute(x + 1, y, inspect, isBlocked, depth);
    compute(x - 1, y, inspect, isBlocked, depth);
    compute(x, y + 1, inspect, isBlocked, depth);
    compute(x, y - 1, inspect, isBlocked, depth);
}