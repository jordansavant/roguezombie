#include "VectorMath.h"
#include "Math.h"

sf::Vector2f bit::VectorMath::GetRandomVector()
{
    float r = (float)std::rand()/(float)RAND_MAX;
    float angle = (r * bit::Math::TwoPi);
    return sf::Vector2f(std::cos(angle), std::sin(angle));
}

sf::Vector2f bit::VectorMath::Normalize(sf::Vector2f &vector)
{
    if(vector.x != 0 || vector.y != 0)
    {
    	float length = std::sqrt((vector.x * vector.x) + (vector.y * vector.y));
	    return sf::Vector2f(vector.x / length, vector.y / length);
    }
    return vector;
}

sf::Vector2f bit::VectorMath::Normalize(float x, float y)
{
    if(x != 0 || y != 0)
    {
		float length = std::sqrt((x * x) + (y * y));
		return sf::Vector2f(x / length, y / length);
    }
    return sf::Vector2f(0, 0);
}

void bit::VectorMath::ApplyDeadZone(sf::Vector2f* v, float deadZone)
{
    if(deadZone > 0)
    {
        if((v->x > 0 && v->x < deadZone) || (v->x < 0 && v->x > -deadZone))
        {
            v->x = 0;
        }
        if((v->y > 0 && v->y < deadZone) || (v->y < 0 && v->y > -deadZone))
        {
            v->y = 0;
        }
    }
}

float bit::VectorMath::Distance(float x1, float y1, float x2, float y2)
{
    float x = (x1 - x2);
    float y = (y1 - y2);
    return std::sqrtf(x*x  + y*y);
}

sf::Vector2f bit::VectorMath::DirectionToVector(float x1, float y1, float x2, float y2)
{
    return Normalize(sf::Vector2f(x2, y2) - sf::Vector2f(x1, y1));
}

float bit::VectorMath::RotationToVector(float x1, float y1, float x2, float y2)
{
    return std::atan2f(y1 - y2, x1 - x2);
}

float bit::VectorMath::ToRadians(sf::Vector2f v)
{
    v = Normalize(v.x, v.y);
    return std::atan2f(v.y, v.x);
}

sf::Vector2f bit::VectorMath::RadiansToVector(float radians)
{
    return sf::Vector2f(std::cosf(radians), std::sinf(radians));
}

sf::Vector2f bit::VectorMath::Average(std::vector<sf::Vector2f> &vectors)
{
    if(vectors.size() == 0)
        return sf::Vector2f(0, 0);

    sf::Vector2f average(0, 0);
    for(int i=0; i < vectors.size(); i++)
    {
        average.x += vectors[i].x;
        average.y += vectors[i].y;
    }
    average.x /= vectors.size();
    average.y /= vectors.size();

    return average;
}

void bit::VectorMath::Rotate(sf::Vector2f &point, float radians)
{
    float cosRadians = std::cosf(radians);
    float sinRadians = std::sinf(radians);

    point.x = point.x * cosRadians - point.y * sinRadians;
    point.y = point.x * sinRadians - point.y * cosRadians;
}

void bit::VectorMath::RotateAroundPivot(sf::Vector2f &point, float radians, sf::Vector2f &pivot)
{
    float cosRadians = std::cosf(radians);
    float sinRadians = std::sinf(radians);

    sf::Vector2f translatedPoint;
    translatedPoint.x = point.x - pivot.x;
    translatedPoint.y = point.y - pivot.y;

    point.x = translatedPoint.x * cosRadians - translatedPoint.y * sinRadians + pivot.x;
    point.y = translatedPoint.x * sinRadians + translatedPoint.y * cosRadians + pivot.y;
}

// h ttp://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm - Algorithm
// h ttp://roguebasin.roguelikedevelopment.org/index.php?title=Bresenham%27s_Line_Algorithm - Rogue Code
std::vector<sf::Vector2i> bit::VectorMath::BresenhamLine(int x1, int y1, int const x2, int const y2)
{
    std::vector<sf::Vector2i> result;

    int delta_x(x2 - x1);
    // if x1 == x2, then it does not matter what we set here
    signed char const ix((delta_x > 0) - (delta_x < 0));
    delta_x = std::abs(delta_x) << 1;

    int delta_y(y2 - y1);
    // if y1 == y2, then it does not matter what we set here
    signed char const iy((delta_y > 0) - (delta_y < 0));
    delta_y = std::abs(delta_y) << 1;

    //plot(x1, y1);
    result.push_back(sf::Vector2i(x1, y1));

    if (delta_x >= delta_y)
    {
        // error may go below zero
        int error(delta_y - (delta_x >> 1));

        while (x1 != x2)
        {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= delta_x;
                y1 += iy;
            }
            // else do nothing

            error += delta_y;
            x1 += ix;

            //plot(x1, y1);
            result.push_back(sf::Vector2i(x1, y1));
        }
    }
    else
    {
        // error may go below zero
        int error(delta_x - (delta_y >> 1));

        while (y1 != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= delta_y;
                x1 += ix;
            }
            // else do nothing

            error += delta_x;
            y1 += iy;

            //plot(x1, y1);
            result.push_back(sf::Vector2i(x1, y1));
        }
    }

    return result;
}


bool bit::VectorMath::inRadius(float x, float y, float radiusX, float radiusY, float radius)
{
    return Distance(x, y, radiusX, radiusY) <= radius;
}
