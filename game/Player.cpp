#include "Player.hpp"
#include "World.hpp"
#include "Character.hpp"

Player::Player()
    : world(NULL), character(NULL), clientId(0)
{
}

void Player::load(unsigned int _clientId)
{
    clientId = _clientId;
}

void Player::setWorld(World* world)
{
    this->world = world;
}

void Player::setCharacter(Character* character)
{
    this->character = character;
}