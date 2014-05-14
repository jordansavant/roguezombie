#include "TileClient.hpp"
#include "Tile.hpp"
#include "LevelClient.hpp"
#include "BodyClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Math.hpp"
#include <sstream>

TileClient::TileClient()
    : schema(), level(NULL), sprite(NULL), quadIndex(0), renderX(0), width(64), height(32), renderY(0), centerRenderX(0), centerRenderY(0) 
{
}

TileClient::~TileClient()
{
}

bool TileClient::isCardinallyAdjacent(BodyClient* body)
{
    float distance = bit::VectorMath::distance(schema.x, schema.y, body->schema.x, body->schema.y);
    return (distance <= schema.width || distance <= schema.height);
}

void TileClient::clientLoad(LevelClient* _level)
{
    level = _level;

    // Game quad
    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Water");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void TileClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(schema.x, schema.y);
    renderX = isoPosition.x - schema.width;
    renderY = isoPosition.y;
    bit::VertexHelper::positionQuad(quad, renderX, renderY, 0, width, height);
    centerRenderX = renderX + width / 2;
    centerRenderY = renderY + height / 2;

    // Test Mouse translation
    float mx = std::floor((float)level->mousePositionInWorld.x / (float)schema.width);
    float my = std::floor((float)level->mousePositionInWorld.y / (float)schema.height);
    float tx = std::floor((float)schema.x / (float)schema.width);
    float ty = std::floor((float)schema.y / (float)schema.height);

    if(level->state->mode == StateGamePlay::Mode::Free && tx == mx && ty == my)
    {
        int s = bit::Math::clamp(255 * schema.illumination * 4, 0, 255);
        sf::Color move(s, s, s);
        sf::Color interact(255, 0, 0);
        bit::VertexHelper::colorQuad(quad, schema.bodyId > 0 ? interact : move);

        // Set me as being hovered over
        level->hoveredTile = this;
    }
    else
    {
        int r = schema.rshade * schema.illumination;
        int g = schema.gshade * schema.illumination;
        int b = schema.bshade * schema.illumination;
        sf::Color c(r, g, b);
        bit::VertexHelper::colorQuad(quad, c);

        // Unset if I was previously set
        if(level->hoveredTile == this)
        {
            level->hoveredTile = NULL;
        }
    }
}

void TileClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void TileClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    packet >> schema;
}