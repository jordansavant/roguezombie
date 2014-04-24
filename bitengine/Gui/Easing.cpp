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

        case Type::InQuad:
            return inQuad(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutQuad:
            return outQuad(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutQuad:
            return inOutQuad(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InCubic:
            return inCubic(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutCubic:
            return outCubic(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutCubic:
            return inOutCubic(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InQuart:
            return inQuart(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutQuart:
            return outQuart(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutQuart:
            return inOutQuart(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InQuint:
            return inQuint(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutQuint:
            return outQuint(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutQuint:
            return inOutQuint(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InSine:
            return inSine(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutSine:
            return outSine(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutSine:
            return inOutSine(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InExpo:
            return inExpo(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutExpo:
            return outExpo(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutExpo:
            return inOutExpo(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InCirc:
            return inCirc(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutCirc:
            return outCirc(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutCirc:
            return inOutCirc(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InElastic:
            return inElastic(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutElastic:
            return outElastic(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutElastic:
            return inOutElastic(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InBack:
            return inBack(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutBack:
            return outBack(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutBack:
            return inOutBack(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);

        case Type::InBounce:
            return inBounce(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::OutBounce:
            return outBounce(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
        case Type::InOutBounce:
            return inOutBounce(percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
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

// easeInQuad: function (x, t, b, c, d) {
//    return c*(t/=d)*t + b;
// }
float bit::Easing::inQuad(float x, float t, float b, float c, float d)
{
    t /= d;
    return c * t * t + b;
}

// easeOutQuad: function (x, t, b, c, d) {
//    return -c *(t/=d)*(t-2) + b;
// }
float bit::Easing::outQuad(float x, float t, float b, float c, float d)
{
    t /= d;
    return -c * t * (t - 2) + b;
}

// easeInOutQuad: function (x, t, b, c, d) {
//    if ((t/=d/2) < 1) return c/2*t*t + b;
//    return -c/2 * ((--t)*(t-2) - 1) + b;
// }
float bit::Easing::inOutQuad(float x, float t, float b, float c, float d)
{
    t /= d / 2;
    if(t < 1)
        return c / 2 * t * t + b;
    --t;
    return -c / 2 * (t * (t-2) -1) + b;
}

// easeInCubic: function (x, t, b, c, d) {
//    return c*(t/=d)*t*t + b;
// }
float bit::Easing::inCubic(float x, float t, float b, float c, float d)
{
    t /= d;
    return c * t * t * t + b;
}

// easeOutCubic: function (x, t, b, c, d) {
//     return c*((t=t/d-1)*t*t + 1) + b;
// }
float bit::Easing::outCubic(float x, float t, float b, float c, float d)
{
    t = t / d - 1;
    return c * (t * t * t + 1) + b;
}

// easeInOutCubic: function (x, t, b, c, d) {
//     if ((t/=d/2) < 1) return c/2*t*t*t + b;
//     return c/2*((t-=2)*t*t + 2) + b;
// }
float bit::Easing::inOutCubic(float x, float t, float b, float c, float d)
{
    t /= d / 2;
    if(t < 1)
        return c / 2 * t * t * t + b;
    t -= 2;
    return c / 2 * (t * t * t + 2) + b;
}

// easeInQuart: function (x, t, b, c, d) {
//     return c*(t/=d)*t*t*t + b;
// }
float bit::Easing::inQuart(float x, float t, float b, float c, float d)
{
    t /= d;
    return c * t * t * t * t + b;
}

// easeOutQuart: function (x, t, b, c, d) {
//     return -c * ((t=t/d-1)*t*t*t - 1) + b;
// },
float bit::Easing::outQuart(float x, float t, float b, float c, float d)
{
    t = t / d - 1;
    return -c * (t * t * t * t - 1) + b;
}

// easeInOutQuart: function (x, t, b, c, d) {
//     if ((t/=d/2) < 1) return c/2*t*t*t*t + b;
//     return -c/2 * ((t-=2)*t*t*t - 2) + b;
// }
float bit::Easing::inOutQuart(float x, float t, float b, float c, float d)
{
    t /= d / 2;
    if(t < 1)
        return c / 2 * t * t * t * t + b;
    t -= 2;
    return -c / 2 * (t * t * t * t - 2) + b;
}

// easeInQuint: function (x, t, b, c, d) {
//    return c*(t/=d)*t*t*t*t + b;
// }
float bit::Easing::inQuint(float x, float t, float b, float c, float d)
{
    t /= d;
    return c * t * t * t * t * t + b;
}

// easeOutQuint: function (x, t, b, c, d) {
//    return c*((t=t/d-1)*t*t*t*t + 1) + b;
// }
float bit::Easing::outQuint(float x, float t, float b, float c, float d)
{
    t = t / d - 1;
    return c * (t * t * t * t * t + 1) + b;
}

// easeInOutQuint: function (x, t, b, c, d) {
//    if ((t/=d/2) < 1) return c/2*t*t*t*t*t + b;
//    return c/2*((t-=2)*t*t*t*t + 2) + b;
// }
float bit::Easing::inOutQuint(float x, float t, float b, float c, float d)
{
    t /= d / 2;
    if(t < 1)
        return c / 2 * t * t * t * t * t + b;
    t -= 2;
    return c / 2 * (t * t * t * t * t + 2) + b;
}

// easeInSine: function (x, t, b, c, d) {
//     return -c * Math.cos(t/d * (Math.PI/2)) + c + b;
// }
float bit::Easing::inSine(float x, float t, float b, float c, float d)
{
    return -c * std::cos(t / d * bit::Math::Pi / 2) + c + b;
}

// easeOutSine: function (x, t, b, c, d) {
//     return c * Math.sin(t/d * (Math.PI/2)) + b;
// }
float bit::Easing::outSine(float x, float t, float b, float c, float d)
{
    return c * std::sin(t / d * bit::Math::Pi / 2) + b;
}

// easeInOutSine: function (x, t, b, c, d) {
//     return -c/2 * (Math.cos(Math.PI*t/d) - 1) + b;
// }
float bit::Easing::inOutSine(float x, float t, float b, float c, float d)
{
    return -c / 2 * (std::cos(bit::Math::Pi * t / d) - 1) + b;
}

// easeInExpo: function (x, t, b, c, d) {
//     return (t==0) ? b : c * Math.pow(2, 10 * (t/d - 1)) + b;
// }
float bit::Easing::inExpo(float x, float t, float b, float c, float d)
{
    return (t == 0) ? b : c * std::pow(2, 10 * (t / d - 1)) + b;
}

// easeOutExpo: function (x, t, b, c, d) {
//     return (t==d) ? b+c : c * (-Math.pow(2, -10 * t/d) + 1) + b;
// }
float bit::Easing::outExpo(float x, float t, float b, float c, float d)
{
    return (t == d) ? b + c : c * (-std::pow(2, -10 * t / d) + 1) + b;
}

// easeInOutExpo: function (x, t, b, c, d) {
//     if (t==0) return b;
//     if (t==d) return b+c;
//     if ((t/=d/2) < 1) return c/2 * Math.pow(2, 10 * (t - 1)) + b;
//     return c/2 * (-Math.pow(2, -10 * --t) + 2) + b;
// }
float bit::Easing::inOutExpo(float x, float t, float b, float c, float d)
{
    if(t == 0)
        return b;
    if(t == d)
        return b + c;
    t /= d / 2;
    if(t < 1)
        return c / 2 * std::pow(2, 10 * (t - 1)) + b;
    return c / 2 * (-std::pow(2, -10 * --t) + 2) + b;
}

// easeInCirc: function (x, t, b, c, d) {
//     return -c * (Math.sqrt(1 - (t/=d)*t) - 1) + b;
// }
float bit::Easing::inCirc(float x, float t, float b, float c, float d)
{
    t /= d;
    return -c * (std::sqrt(1 - t * t) - 1) + b;
}

// easeOutCirc: function (x, t, b, c, d) {
//     return c * Math.sqrt(1 - (t=t/d-1)*t) + b;
// }
float bit::Easing::outCirc(float x, float t, float b, float c, float d)
{
    t = t / d - 1;
    return c * std::sqrt(1 - t * t) + b;
}

// easeInOutCirc: function (x, t, b, c, d) {
//     if ((t/=d/2) < 1) return -c/2 * (Math.sqrt(1 - t*t) - 1) + b;
//     return c/2 * (Math.sqrt(1 - (t-=2)*t) + 1) + b;
// }
float bit::Easing::inOutCirc(float x, float t, float b, float c, float d)
{
    t /= d / 2;
    if(t < 1)
        return -c / 2 * (std::sqrt(1 - t * t) - 1) + b;
    t -= 2;
    return c / 2 * (std::sqrt(1 - t * t) + 1) + b;
}

// easeInElastic: function (x, t, b, c, d) {
//     var s=1.70158;var p=0;var a=c;
//     if (t==0) return b;  if ((t/=d)==1) return b+c;  if (!p) p=d*.3;
//     if (a < Math.abs(c)) { a=c; var s=p/4; }
//     else var s = p/(2*Math.PI) * Math.asin (c/a);
//     return -(a*Math.pow(2,10*(t-=1)) * Math.sin( (t*d-s)*(2*Math.PI)/p )) + b;
// }
float bit::Easing::inElastic(float x, float t, float b, float c, float d)
{
    float s=1.70158;
    float p=d * .3;
    float a=c;

    if (t == 0)
        return b;

    t /= d;
    if (t == 1)
        return b + c;

    if (a < std::abs(c))
    {
        a = c;
        s = p / 4;
    }
    else
    {
        s = p / 2 * bit::Math::Pi * std::asin(c / a);
    }

    t -= 1;
    return -(a * std::pow(2, 10 * t) * std::sin((t * d - s) * (2 * bit::Math::Pi) / p)) + b;
}

// easeOutElastic: function (x, t, b, c, d) {
//     var s=1.70158;var p=0;var a=c;
//     if (t==0) return b;  if ((t/=d)==1) return b+c;  if (!p) p=d*.3;
//     if (a < Math.abs(c)) { a=c; var s=p/4; }
//     else var s = p/(2*Math.PI) * Math.asin (c/a);
//     return a*Math.pow(2,-10*t) * Math.sin( (t*d-s)*(2*Math.PI)/p ) + c + b;
// }
float bit::Easing::outElastic(float x, float t, float b, float c, float d)
{
    float s=1.70158;
    float p=d * .3;
    float a=c;

    if (t==0)
        return b;

    t /= d;
    if (t == 1)
        return b + c;

    if (a < std::abs(c))
    {
       a = c;
       s = p / 4;
    }
    else
    {
        s = p / 2 * bit::Math::Pi * std::asin(c / a);
    }

    return a * std::pow(2, -10 * t) * std::sin(( t * d - s) * (2 * bit::Math::Pi) / p) + c + b;
}

// easeInOutElastic: function (x, t, b, c, d) {
//     var s=1.70158;var p=0;var a=c;
//     if (t==0) return b;  if ((t/=d/2)==2) return b+c;  if (!p) p=d*(.3*1.5);
//     if (a < Math.abs(c)) { a=c; var s=p/4; }
//     else var s = p/(2*Math.PI) * Math.asin (c/a);
//     if (t < 1) return -.5*(a*Math.pow(2,10*(t-=1)) * Math.sin( (t*d-s)*(2*Math.PI)/p )) + b;
//     return a*Math.pow(2,-10*(t-=1)) * Math.sin( (t*d-s)*(2*Math.PI)/p )*.5 + c + b;
// }
float bit::Easing::inOutElastic(float x, float t, float b, float c, float d)
{
    float s=1.70158;
    float p=d * (.3 * 1.5);
    float a=c;

    if (t == 0)
        return b;

    t /= d / 2;
    if (t == 2)
        return b + c;

    if (a < std::abs(c))
    {
        a = c;
        s = p / 4;
    }
    else
    {
        s = p / 2 * bit::Math::Pi * std::asin(c / a);
    }

    if (t < 1)
    {
        t -= 1;
        return -.5 * (a * std::pow(2, 10 * t) * std::sin((t * d - s) * (2 * bit::Math::Pi) / p)) + b;
    }

    t -= 1;
    return a * std::pow(2, -10 * t) * std::sin((t * d - s) * (2 * bit::Math::Pi) / p) * .5 + c + b;
}

// easeInBack: function (x, t, b, c, d, s) {
//     if (s == undefined) s = 1.70158;
//     return c*(t/=d)*t*((s+1)*t - s) + b;
// }
float bit::Easing::inBack(float x, float t, float b, float c, float d)
{
    float s = 1.70158;
    t /= d;
    return c * t * t * ((s + 1) * t - s) + b;
}

// easeOutBack: function (x, t, b, c, d, s) {
//     if (s == undefined) s = 1.70158;
//     return c*((t=t/d-1)*t*((s+1)*t + s) + 1) + b;
// }
float bit::Easing::outBack(float x, float t, float b, float c, float d)
{
    float s = 1.70158;
    t = t / d - 1;
	return c * (t * t * ((s + 1) * t + s) + 1) + b;
}

// easeInOutBack: function (x, t, b, c, d, s) {
//     if (s == undefined) s = 1.70158;
//     if ((t/=d/2) < 1) return c/2*(t*t*(((s*=(1.525))+1)*t - s)) + b;
//     return c/2*((t-=2)*t*(((s*=(1.525))+1)*t + s) + 2) + b;
// }
float bit::Easing::inOutBack(float x, float t, float b, float c, float d)
{
    float s = 1.70158;
    t /= d / 2;
    if (t < 1)
    {
        s *= 1.525;
        return c / 2 * (t * t * ((s + 1) * t - s)) + b;
    }
    t -= 2;
    s *= 1.525;
    return c / 2 * (t * t * ((s + 1) * t + s) + 2) + b;
}

// easeInBounce: function (x, t, b, c, d) {
//     return c - jQuery.easing.easeOutBounce (x, d-t, 0, c, d) + b;
// }
float bit::Easing::inBounce(float x, float t, float b, float c, float d)
{
    return c - outBounce(x, d - t, 0, c, d) + b;
}

// easeOutBounce: function (x, t, b, c, d) {
//     if ((t/=d) < (1/2.75)) {
//         return c*(7.5625*t*t) + b;
//     } else if (t < (2/2.75)) {
//         return c*(7.5625*(t-=(1.5/2.75))*t + .75) + b;
//     } else if (t < (2.5/2.75)) {
//         return c*(7.5625*(t-=(2.25/2.75))*t + .9375) + b;
//     } else {
//         return c*(7.5625*(t-=(2.625/2.75))*t + .984375) + b;
//     }
// }
float bit::Easing::outBounce(float x, float t, float b, float c, float d)
{
    t /= d;
	if (t < (1/2.75))
    {
		return c * (7.5625 * t * t) + b;
	}
    else if (t < (2 / 2.75))
    {
        t -= (1.5 / 2.75);
		return c * (7.5625 * t * t + .75) + b;
	}
    else if (t < (2.5/2.75))
    {
        t -= (2.25 / 2.75);
		return c * (7.5625 * t * t + .9375) + b;
	}
    else
    {
        t = t - (2.625 / 2.75);
		return c * (7.5625 * t * t + .984375) + b;
	}
}

// easeInOutBounce: function (x, t, b, c, d) {
//     if (t < d/2) return jQuery.easing.easeInBounce (x, t*2, 0, c, d) * .5 + b;
//     return jQuery.easing.easeOutBounce (x, t*2-d, 0, c, d) * .5 + c*.5 + b;
// }
float bit::Easing::inOutBounce(float x, float t, float b, float c, float d)
{
    if (t < d / 2)
        return inBounce(x, t * 2, 0, c, d) * .5 + b;
    return outBounce(x, t * 2 - d, 0, c, d) * .5 + c * .5 + b;
}