#pragma once
#ifndef RZ_BODY_H
#define RZ_BODY_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class Level;

class Body
{
public:

    Body();

    enum Type
    {
        None,
        Character,
        Structure
    };

    Level* level;

	struct Schema
	{
        Schema()
            : id(0), type(Type::None), x(0), y(0), width(0), height(0), illumination(0)
        {
        }

        unsigned int id;
        Type type;
		float x, y;
        float width, height;
        float illumination;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.id);
            packet << sf::Uint32(schema.type);
            return packet << schema.x << schema.y << schema.width << schema.height << schema.illumination;
        }

        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.id;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Body::Type>(packet, schema.type);
            return packet >> schema.x >> schema.y >> schema.width >> schema.height >> schema.illumination;
        }
	};
	Schema schema;

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif