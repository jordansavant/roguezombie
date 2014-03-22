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

int bit::Math::floorMod(int value, int precision)
{
    if(std::abs(value) % std::abs(precision) == 0)
        return value;
    else if(value >= 0)
        return value - (value % precision);
    else
        return 0 - ceilMod(std::abs(value), precision);
}

int bit::Math::ceilMod(int value, int precision)
{
    if(std::abs(value) % std::abs(precision) == 0)
        return value;
    else if(value >= 0)
        return value + (precision - (value % precision));
    else
        return 0 - floorMod(std::abs(value), precision);
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