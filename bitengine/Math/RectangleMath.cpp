#include "RectangleMath.hpp"

bool bit::RectangleMath::intersects(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2)
{
    return intersectsPlane(Axis::X, x1, y1, width1, height1, x2, y2, width2, height2) && intersectsPlane(Axis::Y, x1, y1, width1, height1, x2, y2, width2, height2);
}

bool bit::RectangleMath::intersectsPlane(Axis axis, float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2)
{
    float top1 = y1;
    float top2 = y2;
    float bottom1 = y1 + height1;
    float bottom2 = y2 + height2;
    float left1 = x1;
    float left2 = x2;
    float right1 = x1 + width1;
    float right2 = x2 + width2;

    switch(axis)
    {
        case Axis::X:
        {
            bool notInXPlane = (left1 >= right2 || right1 <= left2);
            return !notInXPlane;
        }
        case Axis::Y:
        {
            bool notInYPlane = (bottom1 <= top2 || top1 >= bottom2);
            return !notInYPlane;
        }
    }

    return false;
}

float bit::RectangleMath::axisDistance(Axis axis, float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2)
{
    float top1 = y1;
    float top2 = y2;
    float bottom1 = y1 + height1;
    float bottom2 = y2 + height2;
    float left1 = x1;
    float left2 = x2;
    float right1 = x1 + width1;
    float right2 = x2 + width2;

    switch(axis)
    {
        case Axis::X:
        {
            float xDiffA = std::abs(left1 - right2); // distance between my leftmost X and their rightmost X
            float xDiffB = std::abs(right1 - left2); // distance between my rightmost X and their leftmost X
            float xMin = std::min(xDiffA, xDiffB);
            return xMin;
        }
        case Axis::Y:
        {
            float yDiffA = std::abs(bottom1 - top2); // distance between my bottom and their top
            float yDiffB = std::abs(top1 - bottom2); // distance between my top and their bottom
            float yMin = std::min(yDiffA, yDiffB);
            return yMin;
        }
    }

    return 0;
}

float bit::RectangleMath::distance(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2)
{
    float x = axisDistance(Axis::X, x1, y1, width1, height1, x2, y2, width2, height2);
    float y = axisDistance(Axis::Y, x1, y1, width1, height1, x2, y2, width2, height2);
    return std::sqrt(x*x  + y*y);
}

bool bit::RectangleMath::isCardinallyAdjacent(float x1, float y1, float width1, float height1, float x2, float y2, float width2, float height2)
{
    int xDistance = axisDistance(bit::RectangleMath::Axis::X, x1, y1, width1, height1, x2, y2, width2, height2);
    int yDistance = axisDistance(bit::RectangleMath::Axis::Y, x1, y1, width1, height1, x2, y2, width2, height2);
    bool inYPlane = intersectsPlane(bit::RectangleMath::Axis::Y, x1, y1, width1, height1, x2, y2, width2, height2);
    bool inXPlane = intersectsPlane(bit::RectangleMath::Axis::X, x1, y1, width1, height1, x2, y2, width2, height2);

    return (inXPlane || inYPlane) && (xDistance == 0 || yDistance == 0);
}
