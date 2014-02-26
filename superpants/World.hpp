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

    void load();

    void update(sf::Time &gameTime);

    sf::Packet& compileSnapshot(sf::Packet &packet);

};

#endif