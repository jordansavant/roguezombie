#pragma once
#ifndef BIT_COLORMIXER_H
#define BIT_COLORMIXER_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class ColorMixer
    {
    public:
        ColorMixer();

        ColorMixer(float r, float g, float b, float a);

        ColorMixer(sf::Color &color);

        // values are 0-1 float ratios
        float r, g, b;
        float a;

        // Mixes like light, additively
        void mixAdditive(sf::Color &color);

        void mixAdditive(float newR, float newG, float newB, float newA);

        // Mix average TBD

        // Mix subtractive TBD

        sf::Color toColor();
    };
}

#endif