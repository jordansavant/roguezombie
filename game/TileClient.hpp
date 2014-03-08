#pragma once
#ifndef RZ_TILECLIENT_H
#define RZ_TILECLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "Tile.hpp"
#include <deque>

class WorldClient;

class TileClient : public Tile
{
public:

    TileClient();
    ~TileClient();

    WorldClient* world;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    unsigned int lastSnapshotId;

    void clientLoad(WorldClient* world);

    void clientUpdate(sf::RenderWindow &window, sf::Time &gameTime);

    void reset();
};

#endif