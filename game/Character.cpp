#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "World.hpp"
#include "Tile.hpp"
#include "Player.hpp"

Character::Character()
    : fixedState(), deltaState(), world(NULL), tile(NULL)
{
}

void Character::load(World* _world, Tile* _tile)
{
    world = _world;
    tile = _tile;

	fixedState.maxHealth = 100;
	deltaState.health = 100;
    deltaState.x = tile->fixedState.centerX;
    deltaState.y = tile->fixedState.centerY;
}

void Character::update(sf::Time &gameTime)
{
    Tile* t = world->getTileAtPosition(deltaState.x, deltaState.y);
    if(t)
    {
        tile = t;
    }
}

void Character::posses(Player* player)
{
    fixedState.isPlayerCharacter = true;
    fixedState.clientId = player->clientId;
}

void Character::moveUp()
{
    Tile* t = world->getTileAtPosition(tile->fixedState.x, tile->fixedState.y - tile->fixedState.height);
    moveToTile(t);
}

void Character::moveDown()
{
    Tile* t = world->getTileAtPosition(tile->fixedState.x, tile->fixedState.y + tile->fixedState.height);
    moveToTile(t);
}

void Character::moveLeft()
{
    Tile* t = world->getTileAtPosition(tile->fixedState.x - tile->fixedState.width, tile->fixedState.y);
    moveToTile(t);
}

void Character::moveRight()
{
    Tile* t = world->getTileAtPosition(tile->fixedState.x + tile->fixedState.width, tile->fixedState.y);
    moveToTile(t);
}

void Character::moveToTile(Tile* t)
{
    if(t)
    {
        tile = t;
        deltaState.x = t->fixedState.centerX;
        deltaState.y = t->fixedState.centerY;
    }
}

void Character::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
        packet << fixedState;
    packet << deltaState;
}

void Character::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    if(full)
        packet >> fixedState;
    packet >> deltaState;
}