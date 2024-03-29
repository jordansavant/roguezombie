#pragma once
#ifndef RZ_WALL_H
#define RZ_WALL_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"

class Level;

class Wall : public Structure
{
public:

    Wall();

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);
};

#endif