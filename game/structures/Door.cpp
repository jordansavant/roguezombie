#include "Door.hpp"
#include "../../bitengine/Game.hpp"
#include "SFML/System.hpp"
#include "../World.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"

Door::Door()
    : Structure(), testTimer(7)
{
}

void Door::load(World* _world, unsigned int _id, float _x, float _y)
{
    Structure::load(_world, _id, Structure::Type::Door, _x, _y, _world->tileWidth, _world->tileHeight);

    std::vector<Tile*> currentTiles;
    world->getTilesWithinRectangle(Body::deltaState.x, Body::deltaState.y, Body::deltaState.width, Body::deltaState.height, currentTiles);
    for(unsigned int i=0; i < currentTiles.size(); i++)
    {
        currentTiles[i]->setOccupyingDoor(this);
    }

    deltaState.isOpen = false;
}

void Door::update(sf::Time &gameTime)
{
    Structure::update(gameTime);

    if(testTimer.update(gameTime))
    {
        if(deltaState.isOpen)
            attemptClose();
        else
            attemptOpen();
    }
}

void Door::attemptOpen()
{
    // Unset any tiles I am on as a body
    std::vector<Tile*> currentTiles;
    world->getTilesWithinRectangle(Body::deltaState.x, Body::deltaState.y, Body::deltaState.width, Body::deltaState.height, currentTiles);
    for(unsigned int i=0; i < currentTiles.size(); i++)
    {
        currentTiles[i]->setOccupyingBody(NULL);
    }

    deltaState.isOpen = true;
}

void Door::attemptClose()
{
    bool canClose = true;
    std::vector<Tile*> currentTiles;
    world->getTilesWithinRectangle(Body::deltaState.x, Body::deltaState.y, Body::deltaState.width, Body::deltaState.height, currentTiles);
    for(unsigned int i=0; i < currentTiles.size(); i++)
    {
        if(currentTiles[i]->body)
        {
            // cannot close, there is a body
            canClose = false;
            break;
        }
    }

    if(canClose)
    {
        // Block my tiles
        for(unsigned int i=0; i < currentTiles.size(); i++)
        {
            currentTiles[i]->setOccupyingBody(this);
        }

        deltaState.isOpen = false;
    }
}

void Door::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::prepareSnapshot(packet, full);

    packet << deltaState;
}

void Door::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::handleSnapshot(packet, full);

    packet >> deltaState;
}