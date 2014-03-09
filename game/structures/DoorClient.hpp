#pragma once
#ifndef RZ_DOORCLIENT_H
#define RZ_DOORCLIENT_H

#include "Door.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class WorldClient;

class DoorClient : public Door
{
public:

    DoorClient();

    WorldClient* world;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    unsigned int lastSnapshotId;

    void clientLoad(WorldClient* world);

    void clientUpdate(sf::Time &gameTime);

    void reset();
};

#endif