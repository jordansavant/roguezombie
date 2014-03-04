#include "Player.hpp"
#include "World.hpp"
#include "Character.hpp"

Player::Player()
    : world(NULL), character(NULL), clientId(0)
{
}

void Player::load(World* _world, Character* _character, unsigned int _clientId)
{
	world = _world;
    character = _character;
    clientId = _clientId;
}