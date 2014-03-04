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

void Character::load(World* _world, unsigned int _id, Type _type, float _x, float _y, unsigned int _tileSize)
{
    Body::load(_world, _id, Body::Type::Character, _x, _y, _tileSize);
    fixedState.type = _type;
    fixedState.maxHealth = 100;
	deltaState.health = 100;
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
    Tile* t = world->getTileAtPosition(Body::deltaState.x, Body::deltaState.y - world->tileHeight);
    moveToTile(t);
}

void Character::moveDown()
{
    Tile* t = world->getTileAtPosition(Body::deltaState.x, Body::deltaState.y + world->tileHeight);
    moveToTile(t);
}

void Character::moveLeft()
{
    Tile* t = world->getTileAtPosition(Body::deltaState.x - world->tileWidth, Body::deltaState.y);
    moveToTile(t);
}

void Character::moveRight()
{
    Tile* t = world->getTileAtPosition(Body::deltaState.x + world->tileWidth, Body::deltaState.y);
    moveToTile(t);
}

void Character::moveToTile(Tile* t)
{
    if(t && !t->body)
    {
        Body::deltaState.x = t->fixedState.centerX;
        Body::deltaState.y = t->fixedState.centerY;
    }
    else
    {
        int dooop = 0;
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