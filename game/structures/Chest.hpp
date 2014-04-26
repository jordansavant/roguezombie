#pragma once
#ifndef RZ_CHEST_H
#define RZ_CHEST_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"

class Level;

class Chest : public Structure
{
public:

    Chest();

    struct Schema
    {
        Schema()
            : isLocked(false)
        {
        }

        bool isLocked;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            return packet << schema.isLocked;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            return packet >> schema.isLocked;
        }
    };
    Schema schema;

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif