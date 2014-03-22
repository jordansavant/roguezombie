#pragma once
#ifndef BIT_MATH_H
#define BIT_MATH_H

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

        static int floorMod(int value, int precision);

        static int ceilMod(int value, int precision);

        static float calculateDrawDepth(float y, bool invert = false);

        static float calculateDrawDepth(float y, int layer, int layerCount, bool invert = false);
    };
}

#endif