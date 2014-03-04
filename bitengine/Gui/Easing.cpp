#include "Easing.hpp"
#include "../Math/Math.hpp"
#include <vector>
#include <math.h>
#include <complex>

float bit::Easing::runByType(Type easingType, float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration)
{
    switch(easingType)
    {
        case Type::Instant:
            return instant(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::Linear:
            return linear(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutQuad:
            return outQuad(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InQuad:
            return inQuad(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InQuint:
            return inQuint(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutElastic:
            return outElastic(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutBounce:
            return outBounce(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutBack:
            return outBack(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
    }

    return 0;
}

float bit::Easing::instant(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration)
{
    return endValue;
}

float bit::Easing::linear(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration)
{
    return percentageComplete * endValue;
}

float bit::Easing::outQuad(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration)
{
    return -endValue * percentageComplete * (percentageComplete - 2) + startValue;
}

float bit::Easing::inQuad(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration)
{
    return endValue * percentageComplete * percentageComplete * percentageComplete * percentageComplete + startValue;
}

float bit::Easing::inQuint(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration)
{
    return endValue * percentageComplete * percentageComplete * percentageComplete * percentageComplete * percentageComplete + startValue;
}

float bit::Easing::outElastic(float x, float t, float b, float c, float d)
{
    float s=1.70158;
    float p=0;
    float a=c;

    if (t==0)
    {
        return b;
    }

    if ((t/=d)==1)
    {
        return b+c;
    }

    if (!p)
    {
        p = d * .3;
    }

    if (a < std::abs(c))
    {
        a = c;
    }

    return a * std::pow(2,-10*t) * std::sin( ( t * d - s ) * ( 2 * bit::Math::Pi ) / p ) + c + b;
}

float bit::Easing::outBounce(float x, float t, float b, float c, float d)
{
	if ((t/=d) < (1/2.75)) {
		return c*(7.5625*t*t) + b;
	} else if (t < (2/2.75)) {
        (t-=(1.5/2.75));
		return c * (7.5625 * t * t + .75) + b;
	} else if (t < (2.5/2.75)) {
        (t-=(2.25/2.75));
		return c * (7.5625 * t * t + .9375) + b;
	} else {
        (t = t - (2.625 / 2.75));
		return c * (7.5625 * t * t + .984375) + b;
	}
}

float bit::Easing::outBack(float x, float t, float b, float c, float d)
{
    float s = 1.70158;
    (t=t/d-1);
	return c * (t * t * ((s + 1) * t + s) + 1) + b;
}