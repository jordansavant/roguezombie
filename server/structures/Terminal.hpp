#pragma once
#ifndef RZ_TERMINAL_H
#define RZ_TERMINAL_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Structure.hpp"

class Level;

class Terminal : public Structure
{
public:

    Terminal();

    enum SubType
    {
        None, // 0
        TerminalA, // 1
        ColumnA, // 2
    };

    struct Schema
    {
        Schema()
            : subtype(Terminal::SubType::None)
        {
        }

        SubType subtype;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.subtype);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            bit::NetworkHelper::unpackEnum<sf::Uint32, Terminal::SubType>(packet, schema.subtype);
            return packet;
        }
    };
    Schema schema;

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif