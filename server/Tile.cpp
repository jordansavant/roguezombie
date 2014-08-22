#include "Tile.hpp"
#include "Body.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Math.hpp"

Tile::Tile()
    : bit::Node(0, 0), level(NULL), body(NULL), door(NULL), metadata_shadowcastId(0), metadata_floodfillId(0), schema()
{
}

Tile::~Tile()
{
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

    aStarX = _x;
    aStarY = _y;
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

void Tile::distributedUpdate(sf::Time &gameTime)
{
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

bool Tile::isOccupied()
{
    return body != NULL;
}

bool Tile::hasEntrance(unsigned int entranceId)
{
    for(unsigned int i=0; i < entrances.size(); i++)
    {
        if(entrances[i].id == entranceId)
            return true;
    }

    return false;
}

unsigned int Tile::getEntrancePriority(unsigned int entranceId)
{
    for(unsigned int i=0; i < entrances.size(); i++)
    {
        if(entrances[i].id == entranceId)
            return entrances[i].priority;
    }

    return 100000; // meh low priority, should never happen
}


void Tile::runOnBodyEnter(Body* body)
{
    onBodyEnter.trigger<Tile*, Body*>(this, body);
}

void Tile::runOnBodyLeave(Body* body)
{
    onBodyLeave.trigger<Tile*, Body*>(this, body);
}

void Tile::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    packet << schema;
}