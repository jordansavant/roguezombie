#pragma once
#ifndef RZ_DOOR_H
#define RZ_DOOR_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"

class World;

class Door : public Structure
{
public:

    Door();

    virtual void load(World* world, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);
};

#endif