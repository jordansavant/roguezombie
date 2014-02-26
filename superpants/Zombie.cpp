#include "Zombie.hpp"
#include "World.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Input/InputManager.hpp"
#include "../bitengine/Game/Game.hpp"
#include "../bitengine/Math/Math.hpp"
#include "MultiplayerState.hpp"

Zombie::Zombie()
    : x(0), y(0), direction(0, 0), health(100), maxHealth(100), walkTimer(2), world(NULL)
{
}

void Zombie::load(World* _world, float _x, float _y)
{
    world = _world;
    x = _x;
    y = _y;
}

void Zombie::update(sf::Time &gameTime)
{
    if(walkTimer.update(gameTime))
    {
        direction = bit::VectorMath::GetRandomVector();
    }

    x += direction.x;
    y += direction.y;
}

sf::Packet& Zombie::compileSnapshot(sf::Packet &packet)
{
    return packet << x << y << health;
}

sf::Packet& Zombie::extractSnapshot(sf::Packet &packet)
{
    return packet >> x >> y >> health;
}