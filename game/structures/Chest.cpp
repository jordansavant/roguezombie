#include "Chest.hpp"
#include "../../bitengine/Game.hpp"
#include "SFML/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"
#include <functional>

Chest::Chest()
    : Structure(),  schema()
{
}

void Chest::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Structure::load(_level, _id, Structure::Type::Chest, _x, _y, _level->tileWidth, _level->tileHeight);

    blockFoV = false;
    schema.isLocked = false;
}

void Chest::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}

void Chest::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Structure::prepareSnapshot(packet, full);

    packet << schema;
}