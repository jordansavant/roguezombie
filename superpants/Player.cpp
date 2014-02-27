#include "Player.hpp"
#include "World.hpp"
#include "Zombie.hpp"

Player::Player()
	: world(NULL), zombie(NULL)
{
}

void Player::load(World* _world, Zombie* _zombie)
{
	world = _world;
	zombie = _zombie;
}