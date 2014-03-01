#pragma once
#ifndef RZ_ZOMBIE_H
#define RZ_ZOMBIE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game/GameTimer.hpp"

class World;

class Zombie
{
public:

    Zombie();
    
	struct FixedState
	{
		int maxHealth;
	};
	FixedState fixedState;

	struct DeltaState
	{
		float x, y;
	    sf::Vector2f direction;
		int health;
	};
	DeltaState deltaState;

    bit::GameTimer walkTimer;
    World* world;
	bool isPlayerControlled;

    void load(World* world, float x, float y);

    void update(sf::Time &gameTime);

	void updatePosition(sf::Vector2f &direction);

    virtual sf::Packet& compileSnapshot(sf::Packet& packet);

    virtual sf::Packet& extractSnapshot(sf::Packet& packet);

};

#endif