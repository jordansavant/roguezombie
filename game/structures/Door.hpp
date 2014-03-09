#pragma once
#ifndef RZ_DOOR_H
#define RZ_DOOR_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"

class World;

class Door : public Structure
{
public:

    Door();

    bit::GameTimer testTimer;

    struct DeltaState
    {
        bool isOpen;

        friend sf::Packet& operator <<(sf::Packet& packet, const DeltaState &state)
        {
            return packet << state.isOpen;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, DeltaState &state)
        {
            return packet >> state.isOpen;
        }
    };
    DeltaState deltaState;

    virtual void load(World* world, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);

    virtual void attemptOpen();

    virtual void attemptClose();

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

};

#endif