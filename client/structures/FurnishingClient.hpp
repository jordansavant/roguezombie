#pragma once
#ifndef RZ_FURNISHINGCLIENT_H
#define RZ_FURNISHINGCLIENT_H

#include "../../server/structures/Furnishing.hpp"
#include "../StructureClient.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class LevelClient;

class FurnishingClient : public StructureClient
{
public:

    FurnishingClient();

    Furnishing::Schema schema;
    bit::Sprite* terminalASprite;
    bit::Sprite* terminalBSprite;
    bit::Sprite* columnASprite;
    bit::Sprite* deskASprite;
    bit::Sprite* deskBSprite;
    bit::Sprite* sinkASprite;
    bit::Sprite* sinkBSprite;
    bit::Sprite* opTableASprite;
    unsigned int quadIndex;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    void reset();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif