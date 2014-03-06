#include "ZombieClient.hpp"
#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../GameplayState.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

ZombieClient::ZombieClient()
    : Zombie(), renderX(0), renderY(0)
{
}

void ZombieClient::clientLoad(WorldClient* _world)
{
    world = _world;

    quadIndex = world->vertexMap_01.requestVertexIndex();
    sprite = world->state->game->spriteLoader->getSprite("Zombie");
    sprite->applyToQuad(&world->vertexMap_01.vertexArray[quadIndex]);
}

void ZombieClient::clientUpdate(sf::Time &gameTime)
{
    bit::VectorMath::incrementTowards(renderX, renderY, Body::deltaState.x, Body::deltaState.y, 4);

    float spriteWidth = 21;
    float spriteHeight = 29;
    float xFootOffset = 8;
    float yFootOffset = 5;
    float worldCenterX = renderX + Body::deltaState.width / 2;
    float worldCenterY = renderY + Body::deltaState.height / 2;

    sf::Vector2f r = bit::VectorMath::normalToIsometric(worldCenterX, worldCenterY);
    r.x = r.x - spriteWidth / 2 + xFootOffset / 2;
    r.y = r.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);
    bit::Vertex3* quad = &world->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, r.x, r.y, z, spriteWidth, spriteHeight);
}

void ZombieClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
}

void ZombieClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	Zombie::handleSnapshot(packet, full);

    if(full)
    {
        renderX = Body::deltaState.x;
        renderY = Body::deltaState.y;
    }
}