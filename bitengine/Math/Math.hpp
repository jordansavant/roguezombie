#pragma once
#ifndef BIT_MATH_H
#define BIT_MATH_H

#include <string>

namespace bit
{
    class Math
    {
    public:

        static float PiOver4;
        static float PiOver2;
        static float Pi;
        static float TwoPi;

        static float lerp(float val1, float val2, float ratio);

        static float clamp(float value, float min, float max);

        static float round(float value);

        static float randomFloat();

        static int random(int min, int max);

        static int random(int max);

        static float calculateDrawDepthByRange(float y, float targetYRange, float lowerBoundary = 0, float upperBoundary = 1);

        static float calculateDrawDepth(float y, bool invert = false);

        static float calculateDrawDepth(float y, int layer, int layerCount, bool invert = false);

        static float floorPowerOf2(float value);

        static float ceilPowerOf2(float value);

        static float roundPowerOf2(float value);

        static bool bitwiseHasAny(unsigned int value, unsigned int filter);

        static bool bitwiseHasAll(unsigned int value, unsigned int filter);

        static unsigned int toHash(std::string const&s);

    };
}

#endif