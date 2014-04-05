#pragma once
#ifndef RZ_DOORCLIENT_H
#define RZ_DOORCLIENT_H

#include "Door.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class LevelClient;

class DoorClient : public Door
{
public:

    DoorClient();

    LevelClient* level;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    unsigned int lastSnapshotId;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::RenderWindow &window, sf::Time &gameTime);

    void reset();
};

#endif