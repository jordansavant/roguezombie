#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "World.hpp"
#include "Tile.hpp"

Body::Body()
    : fixedState(), deltaState(), world(NULL)
{
}

void Body::load(World* _world, unsigned int _id, Type _type, float _x, float _y, unsigned int _tileSize)
{
    world = _world;
    fixedState.id = _id;
    deltaState.x = _x;
    deltaState.y = _y;
    deltaState.tileSize = _tileSize;
}

void Body::update(sf::Time &gameTime)
{
    // TODO: Convert to support multi tile bodies

    Tile* tile = world->getTileAtPosition(deltaState.x, deltaState.y);
    if(tile)
    {
        tile->setOccupyingBody(this);
    }
}

void Body::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
        packet << fixedState;
    packet << deltaState;
}

void Body::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
        packet >> fixedState;
    packet >> deltaState;
}