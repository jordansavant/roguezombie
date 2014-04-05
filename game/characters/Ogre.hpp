#pragma once
#ifndef RZ_OGRE_H
#define RZ_OGRE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class Ogre : public Character
{
public:

    Ogre();

    bit::GameTimer walkTimer;

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);
};

#endif