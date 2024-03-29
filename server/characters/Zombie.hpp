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

    virtual void load(Level* level, unsigned int id, float x, float y);
};

#endif