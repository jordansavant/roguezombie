#pragma once
#ifndef RZ_STRUCTURE_H
#define RZ_STRUCTURE_H

#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class Level;
class Player;
class Tile;

class Structure : public Body
{
public:

    Structure();

    enum Type
    {
        None,
        Wall,
        Door,
        Chest,
        Terminal
    };

    struct Schema
    {
        Schema()
            :type(Type::None)
        {
        }

        Type type;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.type);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            bit::NetworkHelper::unpackEnum<sf::Uint32, Structure::Type>(packet, schema.type);
            return packet;
        }
    };
    Schema schema;

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual void setPosition(float x, float y);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif