#include "Player.hpp"
#include "Level.hpp"
#include "Character.hpp"

Player::Player()
    : level(NULL), character(NULL), clientId(0), requestFullSnapshot(false)
{
}

void Player::load(unsigned int _clientId)
{
    clientId = _clientId;
}

void Player::setLevel(Level* level)
{
    this->level = level;
}

void Player::setCharacter(Character* character)
{
    this->character = character;
}