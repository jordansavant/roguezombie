#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "Level.hpp"
#include "Tile.hpp"

Body::Body()
    : level(NULL), fixedState(), deltaState()
{
}

void Body::load(Level* _level, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    level = _level;
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
