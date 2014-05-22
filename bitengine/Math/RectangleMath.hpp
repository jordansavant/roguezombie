#pragma once
#ifndef BIT_RECTANGLEMATH_H
#define BIT_RECTANGLEMATH_H

#include "SFML/Graphics.hpp"

namespace bit
{
	class RectangleMath
	{
	public:

        enum Axis
        {
            X,
            Y
        };

        static bool intersects(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);

        static bool intersectsPlane(Axis axis, float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);

        static float axisDistance(Axis axis, float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);

        static float distance(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2);

	};
}

#endif