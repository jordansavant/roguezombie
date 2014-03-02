#pragma once
#ifndef RZ_ZOMBIE_H
#define RZ_ZOMBIE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class World;

class Zombie
{
public:

    Zombie();

	struct FixedState
	{
		int maxHealth;
        friend sf::Packet& operator <<(sf::Packet& packet, const FixedState &state)
        {
            return packet << state.maxHealth;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, FixedState &state)
        {
            return packet >> state.maxHealth;
        }
	};
	FixedState fixedState;

	struct DeltaState
	{
		float x, y;
	    sf::Vector2f direction;
		int health;
        friend sf::Packet& operator <<(sf::Packet& packet, const DeltaState &state)
        {
            return packet << state.x << state.y << state.health;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, DeltaState &state)
        {
            return packet >> state.x >> state.y >> state.health;
        }
	};
	DeltaState deltaState;

    bit::GameTimer walkTimer;
    World* world;
	bool isPlayerControlled;

    void load(World* world, float x, float y);

    void update(sf::Time &gameTime);

	void updatePosition(sf::Vector2f &direction);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif