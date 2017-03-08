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
    //sf::Vector2f minimapIso = bit::VectorMath::normalToIsometric(x / 4, y / 4);
    sf::Vector2f minimapIso(x / 4, y / 4);
    unsigned int op = 100;

    sf::Color c;
    switch(type)
    {
        case Type::Ground: c = sf::Color(135, 135, 135, op); break;
        case Type::Wall: c = sf::Color(220, 220, 220, op); break;
        case Type::Door: c = sf::Color(255, 150, 0, op); break;
        case Type::Player: c = sf::Color(255, 255, 255, 255); break;
    }

    bit::Vertex3* quad = &minimap->vertexMap.vertexArray[quadIndex];
    sprite->applyToQuad(quad);
    bit::VertexHelper::positionQuad(quad, minimapIso.x, minimapIso.y, z, sprite->width, sprite->height);
    bit::VertexHelper::colorQuad(quad, c);
}

void Minimap::load(Hud* _hud)
{
    hud = _hud;

    markerSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("minimap_mark_square");
    playerSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("minimap_player_square");
    vertexMap.load(&hud->interfaceTexture, sf::PrimitiveType::Quads);

    player.quadIndex = vertexMap.requestVertexIndex();
    player.minimap = this;
    player.sprite = playerSprite;
    player.type = Minimap::Marker::Type::Player;
}

void Minimap::update(sf::Time& gameTime)
{
    float powscale = bit::Math::roundPowerOf2(hud->state->rogueZombieGame->currentResolutionRatio);
    float smoothscale = hud->state->rogueZombieGame->currentResolutionRatio;

    float mapW = 520;
    float smoothW = smoothscale * mapW; // 200 / 300
    float powW = powscale * mapW; // 150 / 300
    float diffW = smoothW - powW; // +-50
    float mapH = 10;
    float smoothH = smoothscale * mapH; // 200 / 300
    float powH = powscale * mapH; // 150 / 300
    float diffH = smoothH - powH; // +-50

    float positionX = (hud->targetWidth - mapW) * smoothscale + diffW;
    float positionY = mapH * smoothscale;

    setScale(powscale, powscale);
    setPosition(positionX, positionY);
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
    player.tileId = tileId;
    player.x = x;
    player.y = y;
    player.draw();
}
