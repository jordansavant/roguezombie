#include "TerminalClient.hpp"
#include "../../server/structures/Terminal.hpp"
#include "SFML/Graphics.hpp"
#include "../LevelClient.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../RZConfig.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

TerminalClient::TerminalClient()
    : StructureClient()
{
}

void TerminalClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_charactersToggleIlluminated.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("TerminalA");
    sprite->applyToQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);
}

void TerminalClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);

    // Position
    float spriteWidth = sprite->width;
    float spriteHeight = sprite->height;
    float xFootOffset = 0;
    float yFootOffset = 16;

    float levelX = BodyClient::schema.x;
    float levelY = BodyClient::schema.y;
    float levelCenterX = levelX + BodyClient::schema.width / 2;
    float levelCenterY = levelY + BodyClient::schema.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    float renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    float renderY = renderPosition.y - spriteHeight + yFootOffset;

    float z = RZConfig::getDrawDepthForGameplay(renderY + spriteHeight);
    bit::Vertex3* quad = &level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, renderX, renderY, z, spriteWidth, spriteHeight);

    // Color and luminence
    int r = BodyClient::schema.rshade * BodyClient::schema.illumination;
    int g = BodyClient::schema.gshade * BodyClient::schema.illumination;
    int b = BodyClient::schema.bshade * BodyClient::schema.illumination;
    sf::Color color(r, g, b);
    bit::VertexHelper::colorQuad(quad, color);
}

void TerminalClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);
}

void TerminalClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    StructureClient::handleSnapshot(packet, full);

    packet >> schema;
}