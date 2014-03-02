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

class World;

class ClientTile : public Tile
{
public:

    ClientTile();

    sf::Sprite renderSprite;
    sf::Texture* renderTexture;

    void clientLoad(sf::Texture* renderTexture);

    void clientUpdate(sf::Time &gameTime);

    void clientDraw(sf::RenderWindow &window, sf::Time &gameTime);
};

#endif