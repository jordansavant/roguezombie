#include "ChestClient.hpp"
#include "Chest.hpp"
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

ChestClient::ChestClient()
    : StructureClient(), lastSnapshotId(0)
{
}

void ChestClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Chest");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void ChestClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    float spriteWidth = 20;
    float spriteHeight = 22;
    float xFootOffset = 0;
    float yFootOffset = 8;

    float levelX = BodyClient::schema.x;
    float levelY = BodyClient::schema.y;
    float levelCenterX = levelX + BodyClient::schema.width / 2;
    float levelCenterY = levelY + BodyClient::schema.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    float renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    float renderY = renderPosition.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(renderY + spriteHeight);
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, renderX, renderY, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color(255 * BodyClient::schema.illumination, 255 * BodyClient::schema.illumination, 255 * BodyClient::schema.illumination);
    bit::VertexHelper::colorQuad(quad, color);
}

void ChestClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void ChestClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    StructureClient::handleSnapshot(packet, full);

    packet >> schema;
}