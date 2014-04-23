#include "Tile.hpp"
#include "Body.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Math.hpp"

Tile::Tile()
    : bit::NodeContainer(), level(NULL), body(NULL), door(NULL), metadata_shadowcastId(0), fixedState(), deltaState()
{
}

Tile::~Tile()
{
    if(node)
    {
        delete node;
    }
}

void Tile::load(Level* _level, unsigned int _id, Type _type, int _x, int _y, int _width, int _height)
{
    level = _level;
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
        body->schema.illumination = 0;
    if(door)
        door->schema.illumination = 0;
}

void Tile::setOccupyingBody(Body* _body)
{
    if(!_body && body)
    {
        runOnBodyLeave(body);

        body = NULL;
        deltaState.bodyId = 0;
    }
    else if(_body)
    {
        body = _body;
        deltaState.bodyId = body->schema.id;

        runOnBodyEnter(body);
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
        deltaState.doorId = body->schema.id;
    }
}

void Tile::runOnBodyEnter(Body* body)
{
    for(unsigned int i=0; i < onBodyEnter.size(); i++)
    {
        onBodyEnter[i](this, body);
    }
}

void Tile::runOnBodyLeave(Body* body)
{
    for(unsigned int i=0; i < onBodyLeave.size(); i++)
    {
        onBodyLeave[i](this, body);
    }
}

void Tile::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    packet << fixedState;
    packet << deltaState;
}

void Tile::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    packet >> fixedState;
    packet >> deltaState;
}
