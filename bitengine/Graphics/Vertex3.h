#pragma once
#ifndef BIT_VERTEX3_H
#define BIT_VERTEX3_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class Vertex3
    {
    public:

        Vertex3();

        Vertex3(const sf::Vector3f& thePosition);

        Vertex3(const sf::Vector3f& thePosition, const sf::Color& theColor);

        Vertex3(const sf::Vector3f& thePosition, const sf::Vector2f& theTexCoords);

        Vertex3(const sf::Vector3f& thePosition, const sf::Color& theColor, const sf::Vector2f& theTexCoords);

        sf::Vector3f position;
        sf::Color color;
        sf::Vector2f texCoords;
    };
}

#endif