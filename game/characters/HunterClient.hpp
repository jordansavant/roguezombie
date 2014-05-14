#pragma once
#ifndef RZ_HUNTERCLIENT_H
#define RZ_HUNTERCLIENT_H

#include "Hunter.hpp"
#include "../CharacterClient.hpp"
#include "../CharacterSprite.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Graphics.hpp"
#include <deque>

class LevelClient;

class HunterClient : public CharacterClient
{
public:

    HunterClient();

    CharacterSprite sprite;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    void reset();
};

#endif