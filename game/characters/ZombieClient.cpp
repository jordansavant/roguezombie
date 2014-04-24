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
    : CharacterClient(), renderX(0), renderY(0), lastSnapshotId(0)
{
}

void ZombieClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Zombie");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void ZombieClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
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
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, r.x, r.y, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color((int)(255.0f * BodyClient::schema.illumination), (int)(255.0f * BodyClient::schema.illumination), (int)(255.0f * BodyClient::schema.illumination));
    bit::VertexHelper::colorQuad(quad, color);
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
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}