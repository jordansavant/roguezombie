#pragma once
#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"

class Zombie;

class World
{
public:

    World();

    ~World();

    std::vector<Zombie*> zombies;
    sf::Texture zombieimage;

    void clientLoad();

    void serverLoad();

    void clientUpdate(sf::Time &gameTime);

    void serverUpdate(sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    sf::Packet& compileSnapshot(sf::Packet &packet);

    sf::Packet& extractSnapshot(sf::Packet &packet);

};

#endif