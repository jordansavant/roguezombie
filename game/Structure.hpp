#pragma once
#ifndef RZ_STRUCTURE_H
#define RZ_STRUCTURE_H

#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class World;
class Player;
class Tile;

class Structure : public Body
{
public:

    Structure();

    enum Type
    {
       Wall,
       Door,
    };

	struct FixedState
	{
        Type type;

        friend sf::Packet& operator <<(sf::Packet& packet, const FixedState &state)
        {
            packet << sf::Uint32(state.type);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, FixedState &state)
        {
            sf::Uint32 type;
            packet >> type;
            state.type = static_cast<Structure::Type>(type);
            return packet;
        }
	};
	FixedState fixedState;

	struct DeltaState
	{
        friend sf::Packet& operator <<(sf::Packet& packet, const DeltaState &state)
        {
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, DeltaState &state)
        {
            return packet;
        }
	};
	DeltaState deltaState;

    virtual void load(World* world, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif