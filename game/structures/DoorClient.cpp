#include "DoorCLient.hpp"
#include "Door.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../GameplayState.hpp"
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

void DoorClient::clientLoad(WorldClient* _world)
{
    world = _world;

    quadIndex = world->vertexMap_01.requestVertexIndex();
    sprite = world->state->game->spriteLoader->getSprite("Door");
    sprite->applyToQuad(&world->vertexMap_01.vertexArray[quadIndex]);
}

void DoorClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&world->vertexMap_01.vertexArray[quadIndex]);

    // Position
    float spriteWidth = 64;
    float spriteHeight = 64;
    float xFootOffset = 0;
    float yFootOffset = 16;

    float worldX = Body::deltaState.x;
    float worldY = Body::deltaState.y;
    float worldCenterX = worldX + Body::deltaState.width / 2;
    float worldCenterY = worldY + Body::deltaState.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(worldCenterX, worldCenterY);
    float renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    float renderY = renderPosition.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(renderY + spriteHeight);
    bit::Vertex3* quad = &world->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, renderX, renderY, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color(255 * Body::deltaState.illumination, 255 * Body::deltaState.illumination, 255 * Body::deltaState.illumination);
    bit::VertexHelper::colorQuad(quad, color);

    if(deltaState.isOpen)
    {
        bit::VertexHelper::colorQuad(quad, sf::Color(color.r, color.g, color.b, 0));
    }
}

void DoorClient::reset()
{
    bit::VertexHelper::resetQuad(&world->vertexMap_01.vertexArray[quadIndex]);
}