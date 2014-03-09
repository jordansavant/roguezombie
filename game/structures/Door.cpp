#include "Door.hpp"
#include "SFML/System.hpp"
#include "../World.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"

Door::Door()
    : Structure()
{
}

void Door::load(World* _world, unsigned int _id, float _x, float _y)
{
    Structure::load(_world, _id, Structure::Type::Door, _x, _y, _world->tileWidth, _world->tileHeight);
}

void Door::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}