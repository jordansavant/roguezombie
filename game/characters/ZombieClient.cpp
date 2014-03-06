#include "ZombieClient.hpp"
#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../GameplayState.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

ZombieClient::ZombieClient()
    : Zombie(), renderX(0), renderY(0)
{
}

void ZombieClient::clientLoad(WorldClient* _world, sf::Texture* texture)
{
    world = _world;
    renderTexture = texture;
    renderSprite.setTexture(*texture);
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

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(worldCenterX, worldCenterY);
    renderPosition.x = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    renderPosition.y = renderPosition.y - spriteHeight + yFootOffset;
    renderSprite.setPosition(renderPosition.x, renderPosition.y);
}

void ZombieClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
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