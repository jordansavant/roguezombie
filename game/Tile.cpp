#include "Tile.hpp"
#include "Body.hpp"

Tile::Tile()
    : fixedState(), deltaState(), world(NULL), body(NULL)
{
}

void Tile::load(World* _world, unsigned int _id, Type _type, int _x, int _y, int _width, int _height)
{
    world = _world;
    fixedState.id = _id;
    fixedState.x = _x;
    fixedState.y = _y;
    fixedState.width = _width;
    fixedState.height = _height;
    fixedState.centerX = _x + _width / 2;
    fixedState.centerY = _y + _height / 2;
}

void Tile::update(sf::Time &gameTime)
{
    setOccupyingBody(NULL);
}

void Tile::setOccupyingBody(Body* _body)
{
    if(!_body)
    {
        body = NULL;
        deltaState.bodyId = 0;
    }
    else
    {
        body = _body;
        deltaState.bodyId = body->fixedState.id;
    }
}

void Tile::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
    {
        packet << fixedState;
    }
    packet << deltaState;
}

void Tile::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
    {
        packet >> fixedState;
    }
    packet >> deltaState;
}