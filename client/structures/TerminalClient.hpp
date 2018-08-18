#pragma once
#ifndef RZ_TERMINALCLIENT_H
#define RZ_TERMINALCLIENT_H

#include "../../server/structures/Terminal.hpp"
#include "../StructureClient.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class LevelClient;

class TerminalClient : public StructureClient
{
public:

    TerminalClient();

    Terminal::Schema schema;
    bit::Sprite* terminalASprite;
    bit::Sprite* columnASprite;
    unsigned int quadIndex;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    void reset();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif