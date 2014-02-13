#include "Sprite.hpp"
#include "SFML/Graphics.hpp"
#include "Vertex3.hpp"
#include "VertexHelper.hpp"

bit::Sprite::Sprite()
    : sf::IntRect(), flip(Flip::None)
{
}

bit::Sprite::Sprite(int left, int top, int width, int height)
    : sf::IntRect(left, top, width, height)
{
}

void bit::Sprite::applyToQuad(Vertex3* quad)
{
    (quad + 0)->texCoords.x = left;
    (quad + 0)->texCoords.y = top;
    (quad + 1)->texCoords.x = left + width;
    (quad + 1)->texCoords.y = top;
    (quad + 2)->texCoords.x = left + width;
    (quad + 2)->texCoords.y = top + height;
    (quad + 3)->texCoords.x = left;
    (quad + 3)->texCoords.y = top + height;

    switch(flip)
    {
        // fill rest as needed
        case Flip::Horizontal:
            VertexHelper::flipQuad(quad, true, false);
            break;
        default:
            break;
    }
}

void bit::Sprite::applyToQuad(sf::Vertex* quad)
{
    quad[0].texCoords.x = left;
    quad[0].texCoords.y = top;

    quad[1].texCoords.x = left + width;
    quad[1].texCoords.y = top;

    quad[2].texCoords.x = left + width;
    quad[2].texCoords.y = top + height;

    quad[3].texCoords.x = left;
    quad[3].texCoords.y = top + height;
}
