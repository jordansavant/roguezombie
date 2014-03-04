#pragma once
#ifndef RZ_PLAYER_H
#define RZ_PLAYER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"

class World;
class Character;

class Player
{
public:

    Player();

    World* world;
	Character* character;
    unsigned int clientId;

    void load(World* world, Character* character, unsigned int clientId);
};

#endif