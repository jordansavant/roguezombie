#pragma once
#ifndef RZ_TILECLIENT_H
#define RZ_TILECLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "Tile.hpp"
#include <deque>

class WorldClient;

class TileClient : public Tile
{
public:

    TileClient();

    WorldClient* world;
    sf::Sprite renderSprite;
    sf::Texture* renderTexture;

    void clientLoad(WorldClient* world, sf::Texture* renderTexture);

    void clientUpdate(sf::RenderWindow &window, sf::Time &gameTime);

    void clientDraw(sf::RenderWindow &window, sf::Time &gameTime);
};

#endif