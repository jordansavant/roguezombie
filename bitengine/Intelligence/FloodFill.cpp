#include "FloodFill.hpp"

void bit::FloodFill::compute(unsigned int x, unsigned int y, std::function<void(int, int)> inspect, std::function<bool(int, int)> isBlocked)
{
    if(isBlocked(x, y))
        return;
    
    inspect(x, y);
    
    compute(x + 1, y, inspect, isBlocked);
    compute(x - 1, y, inspect, isBlocked);
    compute(x, y + 1, inspect, isBlocked);
    compute(x, y - 1, inspect, isBlocked);
}