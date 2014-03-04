#include "Structure.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "World.hpp"
#include "Body.hpp"
#include "Tile.hpp"

Structure::Structure()
    : Body(), fixedState(), deltaState()
{
}

void Structure::load(World* _world, unsigned int _id, Type _type, float _x, float _y, unsigned int _tileSize)
{
    Body::load(_world, _id, Body::Type::Structure, _x, _y, _tileSize);
    fixedState.type = _type;
}

void Structure::update(sf::Time &gameTime)
{
    Body::update(gameTime);
}

void Structure::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::prepareSnapshot(packet, full);

    if(full)
        packet << fixedState;
    packet << deltaState;
}

void Structure::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::handleSnapshot(packet, full);

    if(full)
        packet >> fixedState;
    packet >> deltaState;
}