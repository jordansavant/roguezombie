#include "Spiral.hpp"

bit::Spiral::Spiral()
    : layer(1), leg(0), x(0), y(0)
{
}

void bit::Spiral::next()
{
    switch(leg)
    {
        // Step right
        case 0:

            ++x;

            if(x == layer)
            {
                ++leg;
            }
            break;

        // Step up
        case 1:

            ++y;

            if(y == layer)
            {
                ++leg;
            }
            break;

        // Step left
        case 2:

            --x;

            if(-x == layer)
            {
                ++leg;
            }
            break;

        // Step down
        case 3:

            --y;

            if(-y == layer)
            {
                leg = 0;
                ++layer;
            }
            break;
    }
}
