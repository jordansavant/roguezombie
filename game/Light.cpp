#include "Light.hpp"
#include "World.hpp"
#include "Tile.hpp"
#include "Body.hpp"
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
    bit::Shadowcaster::computeFoV(x / world->tileWidth, y / world->tileHeight, world->tileRows, world->tileColumns, radius,
        std::bind(&Light::setVisible, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        std::bind(&Light::isBlocked, this, std::placeholders::_1, std::placeholders::_2));
}

void Light::setVisible(int x, int y, float distance)
{
    Tile* t = world->getTileAtIndices(x, y);
    if(t && t->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
    {
        t->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;

        float currentLight = t->deltaState.illumination;
        float addition = (.1f + .9f * (1 - distance));
        float combinedLight = currentLight + addition;
        float newLight = bit::Math::clamp(combinedLight, currentLight, 1.0f);
        t->deltaState.illumination = newLight;
    }
}

bool Light::isBlocked(int x, int y)
{
    Tile* t = world->getTileAtIndices(x, y);

    return (t && t->body && t->body->fixedState.type == Body::Type::Structure);
}