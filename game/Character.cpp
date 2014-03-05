#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "World.hpp"
#include "Body.hpp"
#include "Tile.hpp"
#include "Player.hpp"

Character::Character()
    : Body(), fixedState(), deltaState()
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
}

void Character::setControllingPlayer(Player* player)
{
    fixedState.isPlayerCharacter = true;
    fixedState.clientId = player->clientId;
}

void Character::moveUp()
{
    moveToPosition(Body::deltaState.x, Body::deltaState.y - world->tileHeight);
}

void Character::moveDown()
{
    moveToPosition(Body::deltaState.x, Body::deltaState.y + world->tileHeight);
}

void Character::moveLeft()
{
    moveToPosition(Body::deltaState.x - world->tileWidth, Body::deltaState.y);
}

void Character::moveRight()
{
    moveToPosition(Body::deltaState.x + world->tileWidth, Body::deltaState.y);
}

void Character::moveToTile(Tile* t)
{
    moveToPosition(t->fixedState.x, t->fixedState.y);
}

void Character::moveToPosition(float x, float y)
{
    // If I can move to this tile
    //Tile* newTile = world->getTileAtPosition(x, y);
    //Tile* currentTile = world->getTileAtPosition(Body::deltaState.x, Body::deltaState.y);

    //if(newTile && !newTile->body)
    //{
    //    Body::deltaState.x = newTile->fixedState.centerX;
    //    Body::deltaState.y = newTile->fixedState.centerY;
    //  
    //    // Remove from current tile
    //    if(currentTile)
    //    {
    //        currentTile->setOccupyingBody(NULL);
    //    }

    //    // Add to new tile
    //    newTile->setOccupyingBody(this);
    //}

    std::vector<Tile*> newTiles = world->getTilesWithinRectangle(x, y, Body::deltaState.width, Body::deltaState.height);
    std::vector<Tile*> currentTiles = world->getTilesWithinRectangle(Body::deltaState.x, Body::deltaState.y, Body::deltaState.width, Body::deltaState.height);

    // Check if I can move
    bool canMove = true;
    for(unsigned int i=0; i < newTiles.size(); i++)
    {
        if(!newTiles[i] || (newTiles[i]->body && newTiles[i]->body != this))
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