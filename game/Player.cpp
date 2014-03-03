#include "Player.hpp"
#include "World.hpp"
#include "characters/Zombie.hpp"

Player::Player()
	: world(NULL), zombie(NULL), clientId(0)
{
}

void Player::load(World* _world, Zombie* _zombie, unsigned int _clientId)
{
	world = _world;
	zombie = _zombie;
    clientId = _clientId;
}