#pragma once
#ifndef RZ_PLAYER_H
#define RZ_PLAYER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"

class World;
class Zombie;

class Player
{
public:

    Player();

    World* world;
	Zombie* zombie;
    unsigned int clientId;

    void load(World* world, Zombie* zombie, unsigned int clientId);
};

#endif