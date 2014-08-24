#include "DoorClient.hpp"
#include "../../server/structures/Door.hpp"
#include "SFML/Graphics.hpp"
#include "../LevelClient.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

DoorClient::DoorClient()
    : StructureClient()
{
}

void DoorClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_charactersNormal.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Door");
    sprite->applyToQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);
}

void DoorClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);

    // Position
    float spriteWidth = 64;
    float spriteHeight = 64;
    float xFootOffset = 0;
    float yFootOffset = 16;

    float levelX = BodyClient::schema.x;
    float levelY = BodyClient::schema.y;
    float levelCenterX = levelX + BodyClient::schema.width / 2;
    float levelCenterY = levelY + BodyClient::schema.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    float renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    float renderY = renderPosition.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(renderY + spriteHeight);
    bit::Vertex3* quad = &level->vertexMap_charactersNormal.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, renderX, renderY, z, spriteWidth, spriteHeight);

    // Color and luminence
    int r = BodyClient::schema.rshade * BodyClient::schema.illumination;
    int g = BodyClient::schema.gshade * BodyClient::schema.illumination;
    int b = BodyClient::schema.bshade * BodyClient::schema.illumination;
    sf::Color color(r, g, b);
    bit::VertexHelper::colorQuad(quad, color);

    if(schema.isOpen)
    {
        sf::Color dc(color.r, color.g, color.b, 0);
        bit::VertexHelper::colorQuad(quad, dc);
    }
}

void DoorClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);
}

void DoorClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    StructureClient::handleSnapshot(packet, full);

    packet >> schema;
}