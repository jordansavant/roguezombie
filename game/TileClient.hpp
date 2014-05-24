#pragma once
#ifndef RZ_TILECLIENT_H
#define RZ_TILECLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "Tile.hpp"
#include "Body.hpp"
#include <deque>

class LevelClient;
class BodyClient;

class TileClient
{
public:

    TileClient();
    
    virtual ~TileClient();

    Tile::Schema schema;
    LevelClient* level;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    unsigned int lastSnapshotId;
    unsigned int width, height;
    float renderX, renderY;
    float centerRenderX, centerRenderY;
    Body::Type ifBodyThenType;
    bool ifCharacterThenIsDead;

    bool isCardinallyAdjacent(BodyClient* body);
    
    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    void reinitialize();

    void reset();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif