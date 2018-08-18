#pragma once
#ifndef RZ_SCIENTIST_H
#define RZ_SCIENTIST_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class DialogNode;

class Scientist : public Character
{
public:

    Scientist();

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);
};

#endif