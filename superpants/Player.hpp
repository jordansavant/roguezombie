#pragma once
#ifndef RZ_PLAYER_H
#define RZ_PLAYER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game/GameTimer.hpp"

class World;
class Zombie;

class Player
{
public:

    Player();

    World* world;
	Zombie* zombie;

    void load(World* world, Zombie* zombie);
};

#endif