#pragma once
#ifndef RZ_CHEST_H
#define RZ_CHEST_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"
#include "../AccessLevel.hpp"

class Level;
class Character;

class Chest : public Structure
{
public:

    Chest();

    struct Schema
    {
        Schema()
            : isLocked(false), accessLevel(AccessLevel::None)
        {
        }

        bool isLocked;
        AccessLevel accessLevel;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << schema.isLocked;
            packet << sf::Uint32(schema.accessLevel);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.isLocked;
            bit::NetworkHelper::unpackEnum<sf::Uint32, AccessLevel>(packet, schema.accessLevel);
            return packet;
        }
    };
    Schema schema;

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);

    virtual void handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);
    
    virtual void getAvailableInteractions(std::vector<Interaction::Type> &fill);

    virtual void onInventoryOpen(Body* guest = NULL);

    virtual void onInventoryClose(Body* guest = NULL);
};

#endif