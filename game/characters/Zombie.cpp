#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Math.hpp"
#include "../World.hpp"
#include "../Tile.hpp"

Zombie::Zombie()
    : Character(), walkTimer(2), isPlayerControlled(false)
{
}

void Zombie::load(World* _world, Tile* _tile)
{
    Character::load(_world, _tile);
}

void Zombie::update(sf::Time &gameTime)
{
	if(!isPlayerControlled)
	{
		if(walkTimer.update(gameTime))
		{
			deltaState.direction = bit::VectorMath::getRandomVector();
		}

		updatePosition(deltaState.direction);
	}
}

void Zombie::updatePosition(sf::Vector2f &direction)
{
	deltaState.x += direction.x;
	deltaState.y += direction.y;
}

void Zombie::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
    {
        packet << fixedState;
    }
    packet << deltaState;
}

void Zombie::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
    {
        packet >> fixedState;
    }
    packet >> deltaState;
}