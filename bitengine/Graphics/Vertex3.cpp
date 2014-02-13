#include "Vertex3.hpp"
#include "SFML/Graphics.hpp"

bit::Vertex3::Vertex3()
    : position(0, 0, 0), color(255, 255, 255, 255), texCoords(0, 0)
{
}

bit::Vertex3::Vertex3(const sf::Vector3f& thePosition)
    : position(thePosition), color(255, 255, 255, 255), texCoords(0, 0)
{
}

bit::Vertex3::Vertex3(const sf::Vector3f& thePosition, const sf::Color& theColor)
    : position(thePosition), color(theColor), texCoords(0, 0)
{
}

bit::Vertex3::Vertex3(const sf::Vector3f& thePosition, const sf::Vector2f& theTexCoords)
    : position(thePosition), color(255, 255, 255, 255), texCoords(0, 0)
{
}

bit::Vertex3::Vertex3(const sf::Vector3f& thePosition, const sf::Color& theColor, const sf::Vector2f& theTexCoords)
    : position(thePosition), color(theColor), texCoords(0, 0)
{
}