#pragma once
#ifndef RZ_ZOMBIE_H
#define RZ_ZOMBIE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game/GameTimer.hpp"

namespace bit
{
    class ServerPacket;
    class ClientPacket;
}

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

    virtual void prepareSnapshot(bit::ServerPacket &packet);

    virtual void handleSnapshot(bit::ServerPacket &packet);

};

#endif