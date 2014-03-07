#include "Light.hpp"
#include "World.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Intelligence.hpp"
#include <functional>

Light::Light()
    : world(NULL)
{
}

void Light::load(World* _world, float _x, float _y, float _radius)
{
    world = _world;
    x = _x;
    y = _y;
    radius = _radius;
}

void Light::update(sf::Time &gameTime)
{
    bit::Shadowcaster::computeFoV(x / world->tileWidth, y / world->tileHeight, radius,
        std::bind(&World::shadowcastSetVisible, world, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        std::bind(&World::shadowcastIsBlocked, world, std::placeholders::_1, std::placeholders::_2));
}
