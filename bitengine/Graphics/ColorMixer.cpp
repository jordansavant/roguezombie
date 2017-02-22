#include "ColorMixer.hpp"

bit::ColorMixer::ColorMixer()
    : r(0), g(0), b(0), a(0)
{
}

bit::ColorMixer::ColorMixer(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a)
{
}

void bit::ColorMixer::mix(float newR, float newG, float newB, float newA)
{
    float rA = 0;
    float rR = 0;
    float rG = 0;
    float rB = 0;

    // mix colors additively
    if(a != 0 || newA != 0)
    {
        rA = 1 - (1 - newA) * (1 - a);
        rR = newR * newA / rA + r * a * (1 - newA) / rA;
        rG = newG * newA / rA + g * a * (1 - newA) / rA;
        rB = newB * newA / rA + b * a * (1 - newA) / rA;
    }

    a = rA;
    r = rR;
    g = rG;
    b = rB;
}
