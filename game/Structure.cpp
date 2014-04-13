#include "Structure.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "Level.hpp"
#include "Body.hpp"
#include "Tile.hpp"

Structure::Structure()
    : Body(), fixedState(), deltaState()
{
}

void Structure::load(Level* _level, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    Body::load(_level, _id, Body::Type::Structure, _x, _y, _width, _height);
    fixedState.type = _type;

    std::vector<Tile*> currentTiles;
    level->getTilesWithinRectangle(Body::deltaState.x, Body::deltaState.y, Body::deltaState.width, Body::deltaState.height, currentTiles);
    for(unsigned int i=0; i < currentTiles.size(); i++)
    {
        currentTiles[i]->setOccupyingBody(this);
    }
}

void Structure::update(sf::Time &gameTime)
{
    Body::update(gameTime);
}

void Structure::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::prepareSnapshot(packet, full);

    packet << fixedState;
    packet << deltaState;
}

void Structure::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::handleSnapshot(packet, full);

    packet >> fixedState;
    packet >> deltaState;
}