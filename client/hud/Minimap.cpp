#include "Minimap.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

Minimap::Minimap()
{
}

Minimap::Marker::Marker()
    : tileId(0), x(0), y(0), quadIndex(0), minimap(NULL), sprite(NULL), type(Type::Ground)
{
}

void Minimap::Marker::draw()
{
    float z = Hud::getDrawDepth(Hud::zindex_minimap);
    sf::Vector2f minimapIso = bit::VectorMath::normalToIsometric(x / 4, y / 4);

    sf::Color c;
    switch(type)
    {
        case Type::Ground: c = sf::Color(0, 135, 0); break;
        case Type::Wall: c = sf::Color(0, 255, 0); break;
        case Type::Door: c = sf::Color(255, 150, 0); break;
    }
    c.a = 150;

    bit::Vertex3* quad = &minimap->vertexMap.vertexArray[quadIndex];
    sprite->applyToQuad(quad);
    bit::VertexHelper::positionQuad(quad, minimapIso.x, minimapIso.y, z, 16, 9);
    bit::VertexHelper::colorQuad(quad, c);
}

void Minimap::load(Hud* _hud)
{
    hud = _hud;

    markerSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("minimap_mark");
    playerSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("minimap_mark");
    vertexMap.load(&hud->interfaceTexture, sf::PrimitiveType::Quads);



    rect.setPosition(-500, 0);
    rect.setSize(sf::Vector2f(830, 450));
    rect.setOutlineColor(sf::Color(0, 0, 0, 0));
    rect.setOutlineThickness(2);
    rect.setFillColor(sf::Color(0, 0, 0, 0));
}

void Minimap::addPoint(unsigned int tileId, int x, int y, Marker::Type type)
{
    if(points.find(tileId) == points.end())
    {
        Minimap::Marker marker;

        marker.tileId = tileId;
        marker.x = x;
        marker.y = y;
        marker.quadIndex = vertexMap.requestVertexIndex();
        marker.minimap = this;
        marker.sprite = markerSprite;
        marker.type = type;

        marker.draw();

        points.insert(std::pair<unsigned int, Marker>(tileId, marker));
    }
}

void Minimap::setPlayerPosition(unsigned int tileId, int x, int y)
{
}
