#pragma once
#ifndef RZ_OGRECLIENT_H
#define RZ_OGRECLIENT_H

#include "Ogre.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include <deque>

class WorldClient;

class OgreClient : public Ogre
{
public:

    OgreClient();

    WorldClient* world;
    sf::Sprite renderSprite;
    sf::Texture* renderTexture;
    float renderX, renderY;

    void clientLoad(WorldClient* world, sf::Texture* renderTexture);

    void clientUpdate(sf::Time &gameTime);

    void clientDraw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif