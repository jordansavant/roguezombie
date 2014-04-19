#include "Math.hpp"

#include <vector>
#include <random>
#include <math.h>
#include <cmath>
#include <stdio.h>

float bit::Math::PiOver4 = 0.785398163397f;
float bit::Math::PiOver2 = 1.57079632679f;
float bit::Math::Pi = 3.14159265359f;
float bit::Math::TwoPi = 6.28318530718f;

float bit::Math::lerp(float val1, float val2, float ratio)
{
	return val1 + ratio * (val2 - val1);
}

float bit::Math::clamp(float value, float min, float max)
{
    return (value > max ? max : (value < min ? min : value));
}

float bit::Math::round(float value)
{
    return std::floor(value + 0.5);
}

float bit::Math::randomFloat()
{
    return (float)std::rand() / (float)RAND_MAX;
}

int bit::Math::random(int min, int max)
{
    return std::rand() % (max - min) + min;
}

int bit::Math::random(int max)
{
    return std::rand() % max;
}

float bit::Math::calculateDrawDepth(float y, bool invert)
{
    float drawDepth = y;

    if (y == 0)
    {
        drawDepth = (invert ? 0 : 1);
    }
    else if (y > 0)
    {
        drawDepth = (invert ? 1 / y : 1 - 1 / y);
    }
    else
    {
        drawDepth = (invert ? 1 / y : 1 + 1 / y);
    }

    if (drawDepth == 0)
    {
        drawDepth = 1;
    }

    return drawDepth;
}

// This function was ported but not tested
float bit::Math::calculateDrawDepth(float y, int layer, int layerCount, bool invert)
{
    float drawDepth = calculateDrawDepth(invert);

    float _worldLayer = (float)layer;
    float _worldLayerCount = (layerCount + 1);

    float lessImportantDrawDepth = drawDepth / 10.0f;

    float onePointFourStepThingy = 1.0f / _worldLayerCount;
    float slot = onePointFourStepThingy * _worldLayer;
    slot = 1 - slot;

    // Add Z factor
    drawDepth = slot - lessImportantDrawDepth;

    return drawDepth;
}

float bit::Math::floorPowerOf2(float value)
{
    if(value > 1) // 32 bit twiddling
    {
        int v = (int)value;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        return v - (v >> 1);
    }
    else
    {
        // domains: 0, .125, .25, .5, 1
        if(value == 0)
            return 0;
        if(value > 0 && value < .125)
            return 0;
        if(value >= .125 && value < .25)
            return .125;
        if(value >= .25 && value < .5)
            return .25;
        if(value >= .5 && value < 1)
            return .5;
        else
            return 1;
    }
}

float bit::Math::ceilPowerOf2(float value)
{
    if(value > 1) // 32 bit twiddling
    {
        int v = (int)value;
        v--;
        v |= v >> 1;
        v |= v >> 2;
        v |= v >> 4;
        v |= v >> 8;
        v |= v >> 16;
        v++;
        return v;
    }
    else
    {
        // domains: 0, .125, .25, .5, 1
        if(value == 0)
            return 0;
        if(value > 0 && value <= .125)
            return .125;
        if(value > .125 && value <= .25)
            return .25;
        if(value > .25 && value <= .5)
            return .5;
        else
            return 1;
    }
}

float bit::Math::roundPowerOf2(float value)
{
    float ceil = ceilPowerOf2(value);
    float floor = floorPowerOf2(value);
    if(std::abs(ceil - value) < std::abs(value - floor))
        return ceil;
    else
        return floor;
}

bool bit::Math::bitwiseHasAny(unsigned int value, unsigned int filter)
{
    return (value & filter) > 0;
}

bool bit::Math::bitwiseHasAll(unsigned int value, unsigned int filter)
{
    return (value & filter) == filter;
}
