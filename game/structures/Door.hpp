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

    struct DeltaState
    {
        bool isOpen;
        bool isLocked;

        friend sf::Packet& operator <<(sf::Packet& packet, const DeltaState &state)
        {
            return packet << state.isOpen << state.isLocked;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, DeltaState &state)
        {
            return packet >> state.isOpen >> state.isLocked;
        }
    };
    DeltaState deltaState;

    virtual void load(World* world, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);

    virtual void attemptOpen();

    virtual void attemptClose();

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

private:

    void registerTileTriggers(Tile* tile);

};

#endif