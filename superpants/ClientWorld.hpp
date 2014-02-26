#pragma once
#ifndef RZ_CLIENTWORLD_H
#define RZ_CLIENTWORLD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <map>

class ClientZombie;

class ClientWorld
{
public:

    ClientWorld();

    ~ClientWorld();

    std::map<sf::Uint32, ClientZombie*> zombies;
    sf::Texture zombieimage;

    void load();

    void update(sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    sf::Packet& extractSnapshot(sf::Packet &packet);

};

#endif