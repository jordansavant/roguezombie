#pragma once
#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "SFML/Graphics.hpp"

class MultiplayerState;
class Zombie;

class World
{
public:

    World();

    ~World();

    MultiplayerState* state;
    std::vector<Zombie*> zombies;
    sf::Texture zombieimage;

    void load(MultiplayerState* state);

    void update(sf::RenderWindow &window, sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

};

#endif