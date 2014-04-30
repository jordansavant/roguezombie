#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "Character.hpp"

Body::Body()
    : level(NULL), blockFoV(true), schema()
{
}

void Body::load(Level* _level, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    level = _level;
    schema.id = _id;
    schema.type = _type;
    schema.x = _x;
    schema.y = _y;
    schema.width = _width;
    schema.height = _height;
}

void Body::update(sf::Time &gameTime)
{
}

void Body::handleInteraction(Interaction::Type interaction, Body* interactor)
{
}

void Body::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    packet << schema;
}

void Body::prepareInteractionTree(bit::ServerPacket &packet)
{
    packet << sf::Uint32(0);
}