#include "Wall.hpp"
#include "SFML/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"

Wall::Wall()
    : Structure()
{
}

void Wall::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Structure::load(_level, _id, Structure::Type::Wall, _x, _y, _level->tileWidth, _level->tileHeight);
    Body::schema.obstructionRatio = 1;
}

void Wall::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}