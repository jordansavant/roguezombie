#pragma once
#ifndef RZ_WALL_H
#define RZ_WALL_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"

class World;

class Wall : public Structure
{
public:

    Wall();

    virtual void load(World* world, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);
};

#endif