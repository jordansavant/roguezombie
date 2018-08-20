#pragma once
#ifndef RZ_GUARD_H
#define RZ_GUARD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class DialogNode;

class Guard : public Character
{
public:

    Guard();

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);
};

#endif