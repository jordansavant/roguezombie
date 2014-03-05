#include "Wall.hpp"
#include "SFML/System.hpp"
#include "../World.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"

Wall::Wall()
    : Structure()
{
}

void Wall::load(World* _world, unsigned int _id, float _x, float _y)
{
    Structure::load(_world, _id, Structure::Type::Wall, _x, _y, _world->tileWidth, _world->tileHeight);
}

void Wall::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}