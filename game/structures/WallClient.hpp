#pragma once
#ifndef RZ_WALLCLIENT_H
#define RZ_WALLCLIENT_H

#include "Wall.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class WorldClient;

class WallClient : public Wall
{
public:

    WallClient();

    WorldClient* world;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    unsigned int lastSnapshotId;

    void clientLoad(WorldClient* world);

    void clientUpdate(sf::RenderWindow &window, sf::Time &gameTime);

    void reset();
};

#endif