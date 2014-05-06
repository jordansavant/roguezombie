#include "ZombieClient.hpp"
#include "Zombie.hpp"
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

ZombieClient::ZombieClient()
    : CharacterClient(), renderX(0), renderY(0)
{
}

void ZombieClient::clientLoad(LevelClient* _level)
{
    level = _level;

    shadowQuadIndex = level->vertexMap_01.requestVertexIndex();
    shadowSprite = level->state->rogueZombieGame->spriteLoader->getSprite("Zombie_Shadow");
    shadowSprite->applyToQuad(&level->vertexMap_01.vertexArray[shadowQuadIndex]);
    
    bodyQuadIndex = level->vertexMap_01.requestVertexIndex();
    bodySprite = level->state->rogueZombieGame->spriteLoader->getSprite("Zombie_Body");
    bodySprite->applyToQuad(&level->vertexMap_01.vertexArray[bodyQuadIndex]);
    
    frontarmQuadIndex = level->vertexMap_01.requestVertexIndex();
    frontarmSprite = level->state->rogueZombieGame->spriteLoader->getSprite("Zombie_FrontArm");
    frontarmSprite->applyToQuad(&level->vertexMap_01.vertexArray[frontarmQuadIndex]);
    
    headQuadIndex = level->vertexMap_01.requestVertexIndex();
    headSprite = level->state->rogueZombieGame->spriteLoader->getSprite("Zombie_Head");
    headSprite->applyToQuad(&level->vertexMap_01.vertexArray[headQuadIndex]);

}

void ZombieClient::clientUpdate(sf::Time &gameTime)
{
    bit::VectorMath::incrementTowards(renderX, renderY, BodyClient::schema.x, BodyClient::schema.y, 4, 4);

    float spriteWidth = 21;
    float spriteHeight = 29;
    float xFootOffset = 8;
    float yFootOffset = 5;
    float levelCenterX = renderX + BodyClient::schema.width / 2;
    float levelCenterY = renderY + BodyClient::schema.height / 2;

    sf::Vector2f r = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    r.x = r.x - spriteWidth / 2 + xFootOffset / 2;
    r.y = r.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);

    // Position
    bit::VertexHelper::positionQuad(&level->vertexMap_01.vertexArray[headQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::positionQuad(&level->vertexMap_01.vertexArray[frontarmQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::positionQuad(&level->vertexMap_01.vertexArray[bodyQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::positionQuad(&level->vertexMap_01.vertexArray[shadowQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color((int)(255.0f * BodyClient::schema.illumination), (int)(255.0f * BodyClient::schema.illumination), (int)(255.0f * BodyClient::schema.illumination));
    bit::VertexHelper::colorQuad(&level->vertexMap_01.vertexArray[headQuadIndex], color);
    bit::VertexHelper::colorQuad(&level->vertexMap_01.vertexArray[frontarmQuadIndex], color);
    bit::VertexHelper::colorQuad(&level->vertexMap_01.vertexArray[bodyQuadIndex], color);
    bit::VertexHelper::colorQuad(&level->vertexMap_01.vertexArray[shadowQuadIndex], color);

    // Sprite
    headSprite->applyToQuad(&level->vertexMap_01.vertexArray[headQuadIndex]);
    frontarmSprite->applyToQuad(&level->vertexMap_01.vertexArray[frontarmQuadIndex]);
    bodySprite->applyToQuad(&level->vertexMap_01.vertexArray[bodyQuadIndex]);
    shadowSprite->applyToQuad(&level->vertexMap_01.vertexArray[shadowQuadIndex]);
}

void ZombieClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	CharacterClient::handleSnapshot(packet, full);

    if(renderX == 0 && renderY == 0)
    {
        renderX = BodyClient::schema.x;
        renderY = BodyClient::schema.y;
    }
}

void ZombieClient::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[headQuadIndex]);
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[frontarmQuadIndex]);
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[bodyQuadIndex]);
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[shadowQuadIndex]);
}