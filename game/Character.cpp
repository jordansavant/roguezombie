#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "World.hpp"
#include "Tile.hpp"

Character::Character()
    : fixedState(), deltaState(), world(NULL), tile(NULL), isPlayerCharacter(false)
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