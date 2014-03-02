#include "Tile.hpp"

Tile::Tile()
    : fixedState(), deltaState(), world(NULL)
{
}

void Tile::load(World* _world, Type _type, int _x, int _y, int _width, int _height)
{
    world = _world;

    fixedState.x = _x;
    fixedState.y = _y;
    fixedState.width = _width;
    fixedState.height = _height;
    fixedState.centerX = _x + _width / 2;
    fixedState.centerY = _y + _height / 2;
}

void Tile::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
        packet << fixedState;
    packet << deltaState;
}

void Tile::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
        packet >> fixedState;
    packet >> deltaState;
}