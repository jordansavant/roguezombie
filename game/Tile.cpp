#include "Tile.hpp"
#include "Body.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Math.hpp"

Tile::Tile()
    : bit::NodeContainer(), level(NULL), body(NULL), door(NULL), metadata_shadowcastId(0), schema()
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
    schema.id = _id;
    schema.x = _x;
    schema.y = _y;
    schema.width = _width;
    schema.height = _height;
    schema.centerX = _x + _width / 2;
    schema.centerY = _y + _height / 2;
    schema.illumination = 0.0f;
    schema.rshade = 0;
    schema.gshade = 0;
    schema.bshade = 0;

    this->node = new bit::Node(_x, _y, this);
}

void Tile::update(sf::Time &gameTime)
{
    // Reset brightness to none
    schema.illumination = 0.0f;

    // Reset color to black
    schema.rshade = 0;
    schema.gshade = 0;
    schema.bshade = 0;

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
        schema.bodyId = 0;
    }
    else if(_body)
    {
        body = _body;
        schema.bodyId = body->schema.id;

        runOnBodyEnter(body);
    }
}

void Tile::setOccupyingDoor(Body* _door)
{
    if(!_door)
    {
        door = NULL;
        schema.doorId = 0;
    }
    else
    {
        door = _door;
        schema.doorId = body->schema.id;
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
    packet << schema;
}