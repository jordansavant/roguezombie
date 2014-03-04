#pragma once
#ifndef RZ_CHARACTER_H
#define RZ_CHARACTER_H

#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class World;
class Player;
class Tile;

class Character : public Body
{
public:

    Character();

    enum Type
    {
       Zombie
    };

	struct FixedState
	{
		int maxHealth;
        bool isPlayerCharacter;
        unsigned int clientId;
        Type type;

        friend sf::Packet& operator <<(sf::Packet& packet, const Character::FixedState &state)
        {
            packet << sf::Uint32(state.maxHealth) << state.isPlayerCharacter << sf::Uint32(state.clientId) << sf::Uint32(state.type);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Character::FixedState &state)
        {
            sf::Uint32 type;
            packet >> state.maxHealth >> state.isPlayerCharacter >> state.clientId >> type;
            state.type = static_cast<Character::Type>(type);
            return packet;
        }
	};
	FixedState fixedState;

	struct DeltaState
	{
        int health;

        friend sf::Packet& operator <<(sf::Packet& packet, const Character::DeltaState &state)
        {
            return packet << sf::Int32(state.health);
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Character::DeltaState &state)
        {
            return packet >> state.health;
        }
	};
	DeltaState deltaState;


    virtual void load(World* world, unsigned int id, Type type, float x, float y, unsigned int tileSize);

    virtual void update(sf::Time &gameTime);

    virtual void setControllingPlayer(Player* player);

    virtual void moveUp();

    virtual void moveDown();

    virtual void moveLeft();

    virtual void moveRight();

    virtual void moveToTile(Tile* tile);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif