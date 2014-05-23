#pragma once
#ifndef RZ_OGRECLIENT_H
#define RZ_OGRECLIENT_H

#include "Ogre.hpp"
#include "../CharacterClient.hpp"
#include "../CharacterSprite.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Graphics.hpp"
#include <deque>

class LevelClient;

class OgreClient : public CharacterClient
{
public:

    OgreClient();

    void clientLoad(LevelClient* level);

    void clientUpdate(sf::Time &gameTime);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    void reset();
};

#endif