#include "VertexHelper.h"
#include "SFML/Graphics.hpp"
#include "Vertex3.h"

// Could convert to for loop and pass count
// If we want more than quads at any point
void bit::VertexHelper::resetQuad(Vertex3* quad)
{
    (quad + 0)->color = sf::Color::White;
    (quad + 1)->color = sf::Color::White;
    (quad + 2)->color = sf::Color::White;
    (quad + 3)->color = sf::Color::White;

    (quad + 0)->texCoords.x = 0;
    (quad + 0)->texCoords.y = 0;
    (quad + 1)->texCoords.x = 0;
    (quad + 1)->texCoords.y = 0;
    (quad + 2)->texCoords.x = 0;
    (quad + 2)->texCoords.y = 0;
    (quad + 3)->texCoords.x = 0;
    (quad + 3)->texCoords.y = 0;

    (quad + 0)->position.x = 0;
    (quad + 0)->position.y = 0;
    (quad + 0)->position.z = 0;
    (quad + 1)->position.x = 0;
    (quad + 1)->position.y = 0;
    (quad + 1)->position.z = 0;
    (quad + 2)->position.x = 0;
    (quad + 2)->position.y = 0;
    (quad + 2)->position.z = 0;
    (quad + 3)->position.x = 0;
    (quad + 3)->position.y = 0;
    (quad + 3)->position.z = 0;
}

void bit::VertexHelper::positionQuad(Vertex3* quad, float x, float y, float z, float width, float height, float scale)
{
    (quad + 0)->position.x = x;
    (quad + 0)->position.y = y;
    (quad + 0)->position.z = z;

    (quad + 1)->position.x = x + width * scale;
    (quad + 1)->position.y = y;
    (quad + 1)->position.z = z;

    (quad + 2)->position.x = x + width * scale;
    (quad + 2)->position.y = y + height * scale;
    (quad + 2)->position.z = z;

    (quad + 3)->position.x = x;
    (quad + 3)->position.y = y + height * scale;
    (quad + 3)->position.z = z;
}

void bit::VertexHelper::colorQuad(Vertex3* quad, sf::Color &color)
{
    (quad + 0)->color = color;
    (quad + 1)->color = color;
    (quad + 2)->color = color;
    (quad + 3)->color = color;
}

void bit::VertexHelper::rotateQuad(Vertex3* quad, float radians, float centerX, float centerY)
{
    float cosRadians = std::cosf(radians);
    float sinRadians = std::sinf(radians);

    float x = (quad + 0)->position.x - centerX;
    float y = (quad + 0)->position.y - centerY;
    (quad + 0)->position.x = x * cosRadians - y * sinRadians + centerX;
    (quad + 0)->position.y = x * sinRadians + y * cosRadians + centerY;

    x = (quad + 1)->position.x - centerX;
    y = (quad + 1)->position.y - centerY;
    (quad + 1)->position.x = x * cosRadians - y * sinRadians + centerX;
    (quad + 1)->position.y = x * sinRadians + y * cosRadians + centerY;

    x = (quad + 2)->position.x - centerX;
    y = (quad + 2)->position.y - centerY;
    (quad + 2)->position.x = x * cosRadians - y * sinRadians + centerX;
    (quad + 2)->position.y = x * sinRadians + y * cosRadians + centerY;

    x = (quad + 3)->position.x - centerX;
    y = (quad + 3)->position.y - centerY;
    (quad + 3)->position.x = x * cosRadians - y * sinRadians + centerX;
    (quad + 3)->position.y = x * sinRadians + y * cosRadians + centerY;
}

void bit::VertexHelper::flipQuad(Vertex3* quad, bool horizontal, bool vertical)
{
    int ulX = quad[0].texCoords.x;
    int ulY = quad[0].texCoords.y;

    int urX = quad[1].texCoords.x;
    int urY = quad[1].texCoords.y;

    int lrX = quad[2].texCoords.x;
    int lrY = quad[2].texCoords.y;

    int llX = quad[3].texCoords.x;
    int llY = quad[3].texCoords.y;

    if(horizontal)
    {
        quad[0].texCoords.x = urX;
        quad[0].texCoords.y = urY;

        quad[1].texCoords.x = ulX;
        quad[1].texCoords.y = ulY;

        quad[2].texCoords.x = llX;
        quad[2].texCoords.y = llY;

        quad[3].texCoords.x = lrX;
        quad[3].texCoords.y = lrY;
    }
}