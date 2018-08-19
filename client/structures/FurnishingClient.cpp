#include "FurnishingClient.hpp"
#include "../../server/structures/Furnishing.hpp"
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

FurnishingClient::FurnishingClient()
    : StructureClient()
{
}

void FurnishingClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_charactersToggleIlluminated.requestVertexIndex();
    terminalASprite = level->state->rogueZombieGame->spriteLoader->getSprite("TerminalA");
    terminalBSprite = level->state->rogueZombieGame->spriteLoader->getSprite("TerminalB");
    columnASprite = level->state->rogueZombieGame->spriteLoader->getSprite("ColumnA");
    deskASprite = level->state->rogueZombieGame->spriteLoader->getSprite("DeskA");
    deskBSprite = level->state->rogueZombieGame->spriteLoader->getSprite("DeskB");
    sinkASprite = level->state->rogueZombieGame->spriteLoader->getSprite("SinkA");
    sinkBSprite = level->state->rogueZombieGame->spriteLoader->getSprite("SinkB");
    //sprite->applyToQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);
}

void FurnishingClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    bit::Sprite* sprite = NULL;
    switch (schema.subtype) {
        default:
        case Furnishing::SubType::TerminalA:
            sprite = terminalASprite;
            break;
        case Furnishing::SubType::TerminalB:
            sprite = terminalBSprite;
            break;
        case Furnishing::SubType::ColumnA:
            sprite = columnASprite;
            break;
        case Furnishing::SubType::DeskA:
            sprite = deskASprite;
            break;
        case Furnishing::SubType::DeskB:
            sprite = deskBSprite;
            break;
        case Furnishing::SubType::SinkA:
            sprite = sinkASprite;
            break;
        case Furnishing::SubType::SinkB:
            sprite = sinkBSprite;
            break;
    }

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

void FurnishingClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);
}

void FurnishingClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    StructureClient::handleSnapshot(packet, full);

    packet >> schema;
}