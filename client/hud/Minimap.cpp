#include "Minimap.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

Minimap::Minimap()
    : minL(0), maxR(0), minT(0), maxB(0)
{
}

void Minimap::load(Hud* _hud)
{
    hud = _hud;
    sprite = hud->state->rogueZombieGame->spriteLoader->getSprite("minimap_mark");
    vertexMap.load(&hud->interfaceTexture, sf::PrimitiveType::Quads);

    rect.setPosition(0, 0);
    rect.setOutlineColor(sf::Color::Green);
    rect.setOutlineThickness(2);
    rect.setFillColor(sf::Color(0, 0, 0, 0));
}

void Minimap::addPoint(unsigned int tileId, int x, int y)
{
    if(points.find(tileId) == points.end())
    {
        unsigned int quadIndex = vertexMap.requestVertexIndex();
        bit::Vertex3* quad = &vertexMap.vertexArray[quadIndex];
        sprite->applyToQuad(quad);

        sf::Vector2f minimapIso = bit::VectorMath::normalToIsometric(x / 4, y / 4);
        //sf::Vector2f minimapIso(x / 4, y / 4);
        float z = Hud::getDrawDepth(Hud::zindex_minimap);
        bit::VertexHelper::positionQuad(quad, minimapIso.x, minimapIso.y, z, 16, 8);

        points.insert(std::pair<unsigned int, sf::Vector2i>(tileId, sf::Vector2i(x, y)));

        minL = std::min(minL, minimapIso.x - 8);
        maxR = std::max(maxR, minimapIso.x + 8);
        minT = std::min(minT, minimapIso.y - 4);
        maxB = std::max(maxB, minimapIso.y + 4);

        rect.setSize(sf::Vector2f(std::abs(minL) + maxR, std::abs(minT) + maxB));
        rect.setPosition(sf::Vector2f(minL, minT));
    }
}
