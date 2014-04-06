#pragma once
#ifndef RZ_ZOMBIECLIENT_H
#define RZ_ZOMBIECLIENT_H

#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include <deque>

class LevelClient;

class ZombieClient : public Zombie
{
public:

    ZombieClient();

    LevelClient* level;
    float renderX, renderY;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    unsigned int lastSnapshotId;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    void reset();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif