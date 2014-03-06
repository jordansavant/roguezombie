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

void Body::load(World* _world, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    world = _world;
    fixedState.id = _id;
    fixedState.type = _type;
    deltaState.x = _x;
    deltaState.y = _y;
    deltaState.width = _width;
    deltaState.height = _height;
}

void Body::update(sf::Time &gameTime)
{
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