#include "DoorClient.hpp"
#include "Door.hpp"
#include "SFML/Graphics.hpp"
#include "../LevelClient.hpp"
#include "../GameplayState.hpp"
#include "../RogueZombieGame.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

DoorClient::DoorClient()
    : Door(), lastSnapshotId(0)
{
}

void DoorClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Door");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void DoorClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    float spriteWidth = 64;
    float spriteHeight = 64;
    float xFootOffset = 0;
    float yFootOffset = 16;

    float levelX = Body::deltaState.x;
    float levelY = Body::deltaState.y;
    float levelCenterX = levelX + Body::deltaState.width / 2;
    float levelCenterY = levelY + Body::deltaState.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    float renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    float renderY = renderPosition.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(renderY + spriteHeight);
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, renderX, renderY, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color(255 * Body::deltaState.illumination, 255 * Body::deltaState.illumination, 255 * Body::deltaState.illumination);
    bit::VertexHelper::colorQuad(quad, color);

    if(deltaState.isOpen)
    {
        sf::Color dc(color.r, color.g, color.b, 0);
        bit::VertexHelper::colorQuad(quad, dc);
    }
}

void DoorClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}
