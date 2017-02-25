#pragma once
#ifndef RZ_DOOR_H
#define RZ_DOOR_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"

class Level;

class Door : public Structure
{
public:

    Door();

    unsigned int openerCount;

    struct Schema
    {
        Schema()
            : isOpen(true), isLocked(false)
        {
        }

        bool isOpen;
        bool isLocked;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            return packet << schema.isOpen << schema.isLocked;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            return packet >> schema.isOpen >> schema.isLocked;
        }
    };
    Schema schema;

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);

    virtual void setPosition(float x, float y);

    virtual void attemptOpen();

    virtual void attemptClose();

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

private:

    bool previousIsOpen;
    bool currentIsOpen;

    void registerTileTriggers(Tile* tile);
};

#endif