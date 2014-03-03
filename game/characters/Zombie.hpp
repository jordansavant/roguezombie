#pragma once
#ifndef RZ_ZOMBIE_H
#define RZ_ZOMBIE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class Zombie : public Character
{
public:

    Zombie();

    bit::GameTimer walkTimer;

    virtual void load(World* world, Tile* tile);

    virtual void update(sf::Time &gameTime);
};

#endif