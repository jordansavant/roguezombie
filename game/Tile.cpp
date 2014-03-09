#include "Tile.hpp"
#include "Body.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Math.hpp"

Tile::Tile()
    : bit::NodeContainer(), fixedState(), deltaState(), world(NULL), body(NULL), door(NULL), metadata_shadowcastId(0)
{
}

Tile::~Tile()
{
    if(node)
    {
        delete node;
    }
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
    deltaState.illumination = 0.0f;
    deltaState.rshade = 0;
    deltaState.gshade = 0;
    deltaState.bshade = 0;

    this->node = new bit::Node(_x, _y, this);
}

void Tile::update(sf::Time &gameTime)
{
    // Reset brightness to none
    deltaState.illumination = 0.0f;

    // Reset color to black
    deltaState.rshade = 0;
    deltaState.gshade = 0;
    deltaState.bshade = 0;

    // Reset body's illuminance
    if(body)
        body->deltaState.illumination = 0;
    if(door)
        door->deltaState.illumination = 0;
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

void Tile::setOccupyingDoor(Body* _door)
{
    if(!_door)
    {
        door = NULL;
        deltaState.doorId = 0;
    }
    else
    {
        door = _door;
        deltaState.doorId = body->fixedState.id;
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