#pragma once
#ifndef RZ_CLIENTWORLD_H
#define RZ_CLIENTWORLD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <map>

namespace bit
{
    class ServerPacket;
}

class ClientZombie;
class ClientTile;

class ClientWorld
{
public:

    ClientWorld();

    ~ClientWorld();

    std::map<sf::Uint32, ClientZombie*> zombies;
    std::map<sf::Uint32, ClientTile*> tiles;
    sf::Texture zombieimage;
    sf::Texture tileimage;

    void load();

    void update(sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif