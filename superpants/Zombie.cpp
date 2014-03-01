#include "Zombie.hpp"
#include "World.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Input/InputManager.hpp"
#include "../bitengine/Game/Game.hpp"
#include "../bitengine/Game/ServerPacket.hpp"
#include "../bitengine/Game/ClientPacket.hpp"
#include "../bitengine/Math/Math.hpp"
#include "MultiplayerState.hpp"

Zombie::Zombie()
    : fixedState(), deltaState(), walkTimer(2), world(NULL), isPlayerControlled(false)
{
}

void Zombie::load(World* _world, float _x, float _y)
{
    world = _world;

	fixedState.maxHealth = 100;
	deltaState.health = 100;

    deltaState.x = _x;
    deltaState.y = _y;
}

void Zombie::update(sf::Time &gameTime)
{
	if(!isPlayerControlled)
	{
		if(walkTimer.update(gameTime))
		{
			deltaState.direction = bit::VectorMath::GetRandomVector();
		}

		updatePosition(deltaState.direction);
	}
}

void Zombie::updatePosition(sf::Vector2f &direction)
{
	deltaState.x += direction.x;
	deltaState.y += direction.y;
}

void Zombie::prepareSnapshot(bit::ServerPacket &packet)
{
    packet << deltaState.x << deltaState.y << deltaState.health;
}

void Zombie::handleSnapshot(bit::ServerPacket &packet)
{
    packet >> deltaState.x >> deltaState.y >> deltaState.health;
}