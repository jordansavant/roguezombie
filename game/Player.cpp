#include "Player.hpp"
#include "Level.hpp"
#include "Character.hpp"

Player::Player()
    : level(NULL), character(NULL), clientId(0), client(NULL), requestFullSnapshot(false)
{
}

void Player::load(bit::RemoteClient* _client)
{
    client = _client;
    clientId = _client->id;
}

void Player::setLevel(Level* level)
{
    this->level = level;
}

void Player::setCharacter(Character* character)
{
    this->character = character;
}