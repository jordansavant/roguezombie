#include "VertexHelper.hpp"
#include "SFML/Graphics.hpp"
#include "Vertex3.hpp"
#include <vector>
#include <math.h>
#include <cmath>

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

void bit::VertexHelper::rotateQuad(Vertex3* quad, float radians)
{
    float centerX = quad[0].position.x + (quad[1].position.x - quad[0].position.x) / 2;
    float centerY = quad[0].position.y + (quad[2].position.y - quad[0].position.y) / 2;

    rotateQuad(quad, radians, centerX, centerY);
}

void bit::VertexHelper::rotateQuad(Vertex3* quad, float radians, float aroundX, float aroundY)
{
    float cosRadians = std::cos(radians);
    float sinRadians = std::sin(radians);

    float x = (quad + 0)->position.x - aroundX;
    float y = (quad + 0)->position.y - aroundY;
    (quad + 0)->position.x = x * cosRadians - y * sinRadians + aroundX;
    (quad + 0)->position.y = x * sinRadians + y * cosRadians + aroundY;

    x = (quad + 1)->position.x - aroundX;
    y = (quad + 1)->position.y - aroundY;
    (quad + 1)->position.x = x * cosRadians - y * sinRadians + aroundX;
    (quad + 1)->position.y = x * sinRadians + y * cosRadians + aroundY;

    x = (quad + 2)->position.x - aroundX;
    y = (quad + 2)->position.y - aroundY;
    (quad + 2)->position.x = x * cosRadians - y * sinRadians + aroundX;
    (quad + 2)->position.y = x * sinRadians + y * cosRadians + aroundY;

    x = (quad + 3)->position.x - aroundX;
    y = (quad + 3)->position.y - aroundY;
    (quad + 3)->position.x = x * cosRadians - y * sinRadians + aroundX;
    (quad + 3)->position.y = x * sinRadians + y * cosRadians + aroundY;
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

    if(horizontal && !vertical)
    {
        // upper left to upper right
        quad[0].texCoords.x = urX;
        quad[0].texCoords.y = urY;

        // upper right to upper left
        quad[1].texCoords.x = ulX;
        quad[1].texCoords.y = ulY;

        // lower right to lower left
        quad[2].texCoords.x = llX;
        quad[2].texCoords.y = llY;

        // lower left to lower right
        quad[3].texCoords.x = lrX;
        quad[3].texCoords.y = lrY;
    }
    else if(horizontal && vertical)
    {
        // upper left to lower right
        quad[0].texCoords.x = lrX;
        quad[0].texCoords.y = lrY;

        // upper right to lower left
        quad[1].texCoords.x = llX;
        quad[1].texCoords.y = llY;

        // lower right to upper left
        quad[2].texCoords.x = ulX;
        quad[2].texCoords.y = ulY;

        // lower left to upper right
        quad[3].texCoords.x = urX;
        quad[3].texCoords.y = urY;
    }
    else if(!horizontal && vertical)
    {
        // upper left to lower left
        quad[0].texCoords.x = llX;
        quad[0].texCoords.y = llY;

        // upper right to lower right
        quad[1].texCoords.x = lrX;
        quad[1].texCoords.y = lrY;

        // lower right to upper right
        quad[2].texCoords.x = urX;
        quad[2].texCoords.y = urY;

        // lower left to upper left
        quad[3].texCoords.x = ulX;
        quad[3].texCoords.y = ulY;
    }
}