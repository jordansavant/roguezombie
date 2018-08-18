#pragma once
#ifndef BIT_VECTORMATH_H
#define BIT_VECTORMATH_H

#include "SFML/Graphics.hpp"

namespace bit
{
	class VectorMath
	{
	public:

        static sf::Vector2f getRandomVector();

		static sf::Vector2f normalize(sf::Vector2f &vector);

        static sf::Vector2f normalize(float x, float y);

		static sf::Vector2f lerp(float x1, float y1, float x2, float y2, float ratio);

        static void incrementTowards(float &x1, float &y1, float x2, float y2, float xAmount, float yAmount);

        static void functionTowards(float &x, float &y, float startX, float startY, float endX, float endY, float lerpX, float lerpY, float xAmount, float yAmount);

        static void applyDeadZone(sf::Vector2f* v, float deadZone);

        static float distance(float x1, float y1, float x2, float y2);

        static sf::Vector2f directionToVector(float x1, float y1, float x2, float y2);

        static float rotationToVector(float x1, float y1, float x2, float y2);

        static float toRadians(sf::Vector2f v);

        static sf::Vector2f radiansToVector(float radians);

        static sf::Vector2f average(std::vector<sf::Vector2f> &vectors);

        static void rotate(sf::Vector2f &point, float radians);

        static void rotateAroundPivot(sf::Vector2f &point, float radians, sf::Vector2f &pivot);

        static std::vector<sf::Vector2i> bresenhamLine(int x1, int y1, int const x2, int const y2);

        static bool inRadius(float x, float y, float radiusX, float radiusY, float radius);

        static sf::Vector2f normalToIsometric(float x, float y);

        static sf::Vector2f isometricToNormal(float x, float y);
	};
}

#endif