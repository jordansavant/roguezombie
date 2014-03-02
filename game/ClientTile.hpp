#pragma once
#ifndef RZ_CLIENTTILE_H
#define RZ_CLIENTTILE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game/GameTimer.hpp"
#include "Tile.hpp"
#include <deque>

namespace bit
{
    class ServerPacket;
}

class ClientWorld;

class ClientTile : public Tile
{
public:

    ClientTile();

    ClientWorld* world;
    sf::Sprite renderSprite;
    sf::Texture* renderTexture;

    void clientLoad(ClientWorld* world, sf::Texture* renderTexture);

    void clientUpdate(sf::RenderWindow &window, sf::Time &gameTime);

    void clientDraw(sf::RenderWindow &window, sf::Time &gameTime);
};

#endif