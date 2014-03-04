#pragma once
#ifndef RZ_BODY_H
#define RZ_BODY_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class World;

class Body
{
public:

    Body();

    enum Type
    {
        Character,
        Structure
    };

    World* world;

	struct FixedState
	{
        unsigned int id;
        Type type;

        friend sf::Packet& operator <<(sf::Packet& packet, const Body::FixedState &state)
        {
            packet << sf::Uint32(state.id);
            packet << sf::Uint32(state.type);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Body::FixedState &state)
        {
            sf::Uint32 type;
            packet >> state.id;
            packet >> type;
            state.type = static_cast<Body::Type>(type);
            return packet;
        }
	};
	FixedState fixedState;

	struct DeltaState
	{
		float x, y;
        unsigned int tileSize;

        friend sf::Packet& operator <<(sf::Packet& packet, const Body::DeltaState &state)
        {
            return packet << state.x << state.y << sf::Uint32(state.tileSize);
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Body::DeltaState &state)
        {
            return packet >> state.x >> state.y >> state.tileSize;
        }
	};
	DeltaState deltaState;


    virtual void load(World* world, unsigned int id, Type type, float x, float y, unsigned int tileSize);

    virtual void update(sf::Time &gameTime);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif