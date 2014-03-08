#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "World.hpp"
#include "Body.hpp"
#include "Tile.hpp"
#include "Light.hpp"
#include "Player.hpp"

Character::Character()
    : Body(), moveTimer(.75f), fixedState(), deltaState()
{
}

void Character::load(World* _world, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    Body::load(_world, _id, Body::Type::Character, _x, _y, _width, _height);
    fixedState.type = _type;
    fixedState.maxHealth = 100;
	deltaState.health = 100;

    moveToPosition(_x, _y);
}

void Character::update(sf::Time &gameTime)
{
    Body::update(gameTime);

    // Follow designation path points
    if(path.size() > 0 && moveTimer.update(gameTime))
    {
        Tile* nextTile = path.back();

        if(nextTile->fixedState.x != Body::deltaState.x || nextTile->fixedState.y != Body::deltaState.y)
        {
            if(moveToTile(nextTile))
            {
                path.pop_back();
            }
        }
        else
        {
            path.pop_back();
        }
    }

    // If I am a player character and I have a light, update it
    if(fixedState.isPlayerCharacter)
    {
        for(unsigned int i=0; i < lights.size(); i++)
        {
            lights[i]->x = Body::deltaState.x;
            lights[i]->y = Body::deltaState.y;
        }
    }
}

void Character::setControllingPlayer(Player* player)
{
    fixedState.isPlayerCharacter = true;
    fixedState.clientId = player->clientId;
}

bool Character::moveUp()
{
    return moveToPosition(Body::deltaState.x, Body::deltaState.y - world->tileHeight);
}

bool Character::moveDown()
{
    return moveToPosition(Body::deltaState.x, Body::deltaState.y + world->tileHeight);
}

bool Character::moveLeft()
{
    return moveToPosition(Body::deltaState.x - world->tileWidth, Body::deltaState.y);
}

bool Character::moveRight()
{
    return moveToPosition(Body::deltaState.x + world->tileWidth, Body::deltaState.y);
}

bool Character::moveToTile(Tile* t)
{
    return moveToPosition(t->fixedState.x, t->fixedState.y);
}

bool Character::moveToPosition(float x, float y)
{
    std::vector<Tile*> newTiles = world->getTilesWithinRectangle(x, y, Body::deltaState.width, Body::deltaState.height);
    std::vector<Tile*> currentTiles = world->getTilesWithinRectangle(Body::deltaState.x, Body::deltaState.y, Body::deltaState.width, Body::deltaState.height);

    // Check if I can move
    bool canMove = true;
    for(unsigned int i=0; i < newTiles.size(); i++)
    {
        if(isTileBlocked(newTiles[i]))
        {
            canMove = false;
            break;
        }
    }
    if(canMove)
    {
        for(unsigned int i=0; i < currentTiles.size(); i++)
        {
            currentTiles[i]->setOccupyingBody(NULL);
        }
        for(unsigned int i=0; i < newTiles.size(); i++)
        {
            newTiles[i]->setOccupyingBody(this);
        }

        Body::deltaState.x = x;
        Body::deltaState.y = y;
    }

    return canMove;
}

void Character::pathToPosition(float x, float y)
{
    path.clear();
    path = world->getShortestPath(Body::deltaState.x, Body::deltaState.y, x, y, std::bind(&Character::isTileBlockedForPathfinding, this, std::placeholders::_1), std::bind(&World::getCardinalTiles, world, std::placeholders::_1));
}

bool Character::isTileBlocked(Tile* tile)
{
    return (!tile || (tile->body && tile->body != this));
}

bool Character::isTileBlockedForPathfinding(Tile* tile)
{
    // Look at all tiles within my width and height
    std::vector<Tile*> tiles = world->getTilesWithinRectangle(tile->fixedState.x, tile->fixedState.y, Body::deltaState.width, Body::deltaState.height);
    for(unsigned int i=0; i < tiles.size(); i++)
    {
        if(isTileBlocked(tiles[i]))
        {
            return true;
        }
    }

    return false;
}

void Character::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::prepareSnapshot(packet, full);

    if(full)
        packet << fixedState;
    packet << deltaState;
}

void Character::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    Body::handleSnapshot(packet, full);

    if(full)
        packet >> fixedState;
    packet >> deltaState;
}