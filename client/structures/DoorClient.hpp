#pragma once
#ifndef RZ_DOORCLIENT_H
#define RZ_DOORCLIENT_H

#include "../../server/structures/Door.hpp"
#include "../StructureClient.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class LevelClient;

class DoorClient : public StructureClient
{
public:

    DoorClient();

    Door::Schema schema;
    bit::Sprite* sprite;
    unsigned int quadIndex;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    void reset();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif