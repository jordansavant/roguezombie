#pragma once
#ifndef BIT_COLORMIXER_H
#define BIT_COLORMIXER_H

namespace bit
{
    class ColorMixer
    {
    public:
        ColorMixer();

        ColorMixer(float r, float g, float b, float a);

        // values are 0-1 float ratios
        float r, g, b;
        float a;

        void mix(float newR, float newG, float newB, float newA);
    };
}

#endif