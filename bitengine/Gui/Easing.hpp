#pragma once
#ifndef BIT_EASINGS_H
#define BIT_EASINGS_H

namespace bit
{
    // jQuery based easings
    class Easing
    {
    public:
        enum Type
        {
            Instant,
            Linear,
            OutQuad,
            InQuad,
            InQuint,
            OutElastic,
            OutBounce,
            OutBack,
        };

        static float runByType(Type easingType, float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float instant(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float linear(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float outQuad(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inQuad(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inQuint(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float outElastic(float x, float t, float b, float c, float d);

        static float outBounce(float x, float t, float b, float c, float d);

        static float outBack(float x, float t, float b, float c, float d);
    };
}

#endif