#pragma once
#ifndef RZ_CHESTCLIENT_H
#define RZ_CHESTCLIENT_H

#include "../../server/structures/Chest.hpp"
#include "../StructureClient.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class LevelClient;

class ChestClient : public StructureClient
{
public:

    ChestClient();

    Chest::Schema schema;
    bit::Sprite* spriteDefault;
    bit::Sprite* spriteYellow;
    unsigned int quadIndex;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    void reset();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif