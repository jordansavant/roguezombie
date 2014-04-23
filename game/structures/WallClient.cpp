#include "WallClient.hpp"
#include "Wall.hpp"
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

WallClient::WallClient()
    : Wall(), lastSnapshotId(0)
{
}

void WallClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Wall");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void WallClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    float spriteWidth = 64;
    float spriteHeight = 64;
    float xFootOffset = 0;
    float yFootOffset = 16;

    float levelX = Body::schema.x;
    float levelY = Body::schema.y;
    float levelCenterX = levelX + Body::schema.width / 2;
    float levelCenterY = levelY + Body::schema.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    float renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    float renderY = renderPosition.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(renderY + spriteHeight);
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, renderX, renderY, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color(255 * Body::schema.illumination, 255 * Body::schema.illumination, 255 * Body::schema.illumination);
    bit::VertexHelper::colorQuad(quad, color);
}

void WallClient::reset()
{
     bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}