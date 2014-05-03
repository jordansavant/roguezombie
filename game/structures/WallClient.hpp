#pragma once
#ifndef RZ_WALLCLIENT_H
#define RZ_WALLCLIENT_H

#include "Wall.hpp"
#include "../StructureClient.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class LevelClient;

class WallClient : public StructureClient
{
public:

    WallClient();

    bit::Sprite* sprite;
    unsigned int quadIndex;

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    void reset();
};

#endif