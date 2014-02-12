#pragma once
#ifndef BIT_MATHHELPER_H
#define BIT_MATHHELPER_H

namespace bit
{
    class Math
    {
    public:
        
        static float PiOver4;
        static float PiOver2;
        static float Pi;
        static float TwoPi;

        static float clamp(float value, float min, float max);

        static float round(float value);

        static float randomFloat();

        static int random(int min, int max);

        static int random(int max);

        static float calculateDrawDepth(float y, bool invert = false);

        static float calculateDrawDepth(float y, int layer, int layerCount, bool invert = false);
    };
}

#endif