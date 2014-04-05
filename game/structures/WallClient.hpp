#pragma once
#ifndef RZ_WALLCLIENT_H
#define RZ_WALLCLIENT_H

#include "Wall.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class LevelClient;

class WallClient : public Wall
{
public:

    WallClient();

    LevelClient* level;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    unsigned int lastSnapshotId;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::RenderWindow &window, sf::Time &gameTime);

    void reset();
};

#endif