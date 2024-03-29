#include "ChestClient.hpp"
#include "../../server/structures/Chest.hpp"
#include "../../server/AccessLevel.hpp"
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

ChestClient::ChestClient()
    : StructureClient()
{
}

void ChestClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_charactersToggleIlluminated.requestVertexIndex();
    spriteDefault = level->state->rogueZombieGame->spriteLoader->getSprite("Chest");
    spriteYellow = level->state->rogueZombieGame->spriteLoader->getSprite("Chest_Yellow");
    //spriteDefault->applyToQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);
}

void ChestClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    bit::Sprite* sprite = NULL;
    switch (schema.accessLevel) {
        default:
            sprite = spriteDefault;
            break;
        case AccessLevel::Yellow:
            sprite = spriteYellow;
            break;
    }
    // Sprite
    sprite->applyToQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);

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

void ChestClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_charactersToggleIlluminated.vertexArray[quadIndex]);
}

void ChestClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    StructureClient::handleSnapshot(packet, full);

    packet >> schema;
}