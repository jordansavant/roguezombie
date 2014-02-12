#pragma once
#ifndef BIT_VECTORHELPER_H
#define BIT_VECTORHELPER_H

#include "SFML\Graphics.hpp"

namespace bit
{
	class VectorMath
	{
	public:

        static sf::Vector2f GetRandomVector();

		static sf::Vector2f Normalize(sf::Vector2f &vector);

        static sf::Vector2f Normalize(float x, float y);

        static void ApplyDeadZone(sf::Vector2f* v, float deadZone);

        static float Distance(float x1, float y1, float x2, float y2);

        static sf::Vector2f DirectionToVector(float x1, float y1, float x2, float y2);

        static float RotationToVector(float x1, float y1, float x2, float y2);

        static float ToRadians(sf::Vector2f v);

        static sf::Vector2f RadiansToVector(float radians);

        static sf::Vector2f Average(std::vector<sf::Vector2f> &vectors);

        static void Rotate(sf::Vector2f &point, float radians);

        static void RotateAroundPivot(sf::Vector2f &point, float radians, sf::Vector2f &pivot);

        static std::vector<sf::Vector2i> BresenhamLine(int x1, int y1, int const x2, int const y2);

        static bool inRadius(float x, float y, float radiusX, float radiusY, float radius);
	};
}

#endif