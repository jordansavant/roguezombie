#pragma once
#ifndef RZ_CHARACTER_H
#define RZ_CHARACTER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class World;
class Tile;
class Player;

class Character
{
public:

    Character();

	struct FixedState
	{
        FixedState()
            : maxHealth(0), isPlayerCharacter(false), clientId(0)
        {
        }

		sf::Int32 maxHealth;
        bool isPlayerCharacter;
        bool clientId;

        friend sf::Packet& operator <<(sf::Packet& packet, const FixedState &state)
        {
            return packet << state.maxHealth << state.isPlayerCharacter << state.clientId;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, FixedState &state)
        {
            return packet >> state.maxHealth >> state.isPlayerCharacter >> state.clientId;
        }
	};
	FixedState fixedState;

	struct DeltaState
	{
		float x, y;
	    sf::Vector2f direction;
        sf::Int32 health;

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

    World* world;
    Tile* tile;

    virtual void load(World* world, Tile* tile);

    virtual void update(sf::Time &gameTime);

    virtual void posses(Player* player);

    virtual void moveUp();

    virtual void moveDown();

    virtual void moveLeft();

    virtual void moveRight();

    virtual void moveToTile(Tile* tile);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif