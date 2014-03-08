#pragma once
#ifndef RZ_TIGHT_H
#define RZ_TIGHT_H

#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Intelligence.hpp"

class World;

class Light
{
public:

    Light();

    World* world;
    float x, y, radius;

    virtual void load(World* world, float x, float y, float radius);

    virtual void update(sf::Time &gameTime);

private:

    virtual void setVisible(int x, int y, float distance);

    virtual bool isBlocked(int x, int y);
};

#endif