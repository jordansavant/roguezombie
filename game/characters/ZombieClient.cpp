#include "ZombieClient.hpp"
#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../LevelClient.hpp"
#include "../GameplayState.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

ZombieClient::ZombieClient()
    : Zombie(), renderX(0), renderY(0), lastSnapshotId(0)
{
}

void ZombieClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->game->spriteLoader->getSprite("Zombie");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void ZombieClient::clientUpdate(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    bit::VectorMath::incrementTowards(renderX, renderY, Body::deltaState.x, Body::deltaState.y, 4, 4);

    float spriteWidth = 21;
    float spriteHeight = 29;
    float xFootOffset = 8;
    float yFootOffset = 5;
    float levelCenterX = renderX + Body::deltaState.width / 2;
    float levelCenterY = renderY + Body::deltaState.height / 2;

    sf::Vector2f r = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    r.x = r.x - spriteWidth / 2 + xFootOffset / 2;
    r.y = r.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, r.x, r.y, z, spriteWidth, spriteHeight);

    // Color and luminence
    float i = Body::deltaState.illumination;
    sf::Color color((int)(255.0f * Body::deltaState.illumination), (int)(255.0f * Body::deltaState.illumination), (int)(255.0f * Body::deltaState.illumination));
    bit::VertexHelper::colorQuad(quad, color);
}

void ZombieClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	Zombie::handleSnapshot(packet, full);

    if(renderX == 0 && renderY == 0)
    {
        renderX = Body::deltaState.x;
        renderY = Body::deltaState.y;
    }
}

void ZombieClient::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}