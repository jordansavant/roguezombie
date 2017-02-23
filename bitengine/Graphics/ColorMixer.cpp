#include "ColorMixer.hpp"

bit::ColorMixer::ColorMixer()
    : r(0), g(0), b(0), a(0)
{
}

bit::ColorMixer::ColorMixer(float r, float g, float b, float a)
    : r(r), g(g), b(b), a(a)
{
}

bit::ColorMixer::ColorMixer(sf::Color &color)
    : r((float)color.r / 255), g((float)color.g / 255), b((float)color.b / 255), a((float)color.a / 255)
{
}

void bit::ColorMixer::mixAdditive(sf::Color &color)
{
    return mixAdditive((float)color.r / 255, (float)color.g / 255, (float)color.b / 255, (float)color.a / 255);
}

void bit::ColorMixer::mixAdditive(float newR, float newG, float newB, float newA)
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

sf::Color bit::ColorMixer::toColor()
{
    return sf::Color(r * 255, g * 255, b * 255, a * 255);
}