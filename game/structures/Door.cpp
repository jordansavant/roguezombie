#include "Door.hpp"
#include "../../bitengine/Game.hpp"
#include "SFML/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"
#include <functional>

Door::Door()
    : Structure(), openerCount(0)
{
}

void Door::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Structure::load(_level, _id, Structure::Type::Door, _x, _y, _level->tileWidth, _level->tileHeight);

    std::vector<Tile*> currentTiles;
    level->getTilesWithinRectangle(Body::schema.x, Body::schema.y, Body::schema.width, Body::schema.height, currentTiles);
    for(unsigned int i=0; i < currentTiles.size(); i++)
    {
        currentTiles[i]->setOccupyingDoor(this);
    }

    std::vector<Tile*> cardinalTiles;
    level->getCardinalTiles(level->getTileAtPosition(_x, _y), cardinalTiles);
    for(unsigned int i=0; i < cardinalTiles.size(); i++)
    {
        registerTileTriggers(cardinalTiles[i]);
    }

    schema.isOpen = false;
    schema.isLocked = false;
}

void Door::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}

void Door::attemptOpen()
{
    // Unset any tiles I am on as a body
    std::vector<Tile*> currentTiles;
    level->getTilesWithinRectangle(Body::schema.x, Body::schema.y, Body::schema.width, Body::schema.height, currentTiles);
    for(unsigned int i=0; i < currentTiles.size(); i++)
    {
        if(currentTiles[i]->body && currentTiles[i]->body == this)
        {
            currentTiles[i]->setOccupyingBody(NULL);
        }
    }

    schema.isOpen = true;
}

void Door::attemptClose()
{
    bool canClose = true;
    std::vector<Tile*> currentTiles;
    level->getTilesWithinRectangle(Body::schema.x, Body::schema.y, Body::schema.width, Body::schema.height, currentTiles);
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

        schema.isOpen = false;
    }
}

void Door::registerTileTriggers(Tile* tile)
{
    Door* d = this;

    if(tile)
    {
        tile->onBodyEnter.push_back([d] (Tile* t, Body* b) {
            if(b->Body::schema.type == Body::Type::Character)
            {
                d->openerCount++;
                d->attemptOpen();
            }
        });
        tile->onBodyLeave.push_back([d] (Tile* t, Body* b) {
            if(b->Body::schema.type == Body::Type::Character)
            {
                d->openerCount--;
                if(d->openerCount == 0)
                {
                    d->attemptClose();
                }
            }
        });
    }
}

void Door::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Structure::prepareSnapshot(packet, full);

    packet << schema;
}