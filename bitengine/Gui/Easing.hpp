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

            InQuad,
            OutQuad,
            InOutQuad,

            InCubic,
            OutCubic,
            InOutCubic,

            InQuart,
            OutQuart,
            InOutQuart,

            InQuint,
            OutQuint,
            InOutQuint,

            InSine,
            OutSine,
            InOutSine,

            InExpo,
            OutExpo,
            InOutExpo,

            InCirc,
            OutCirc,
            InOutCirc,

            InElastic,
            OutElastic,
            InOutElastic,

            InBack,
            OutBack,
            InOutBack,

            InBounce,
            OutBounce,
            InOutBounce
        };

        static float runByType(Type easingType, float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float instant(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float linear(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inQuad(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outQuad(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutQuad(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inCubic(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outCubic(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutCubic(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inQuart(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outQuart(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutQuart(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inQuint(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outQuint(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutQuint(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inSine(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outSine(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutSine(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inExpo(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outExpo(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutExpo(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inCirc(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outCirc(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutCirc(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inElastic(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outElastic(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutElastic(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inBack(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outBack(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutBack(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);

        static float inBounce(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float outBounce(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
        static float inOutBounce(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
    };
}

#endif