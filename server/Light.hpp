#pragma once
#ifndef RZ_TIGHT_H
#define RZ_TIGHT_H

#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Intelligence.hpp"
#include "LevelRunnable.hpp"

class Level;
class Body;

class Light : public LevelRunnable
{
public:

    Light();

    Level* level;
    float x, y, radius;
    sf::Color color;
    float brightness;

    virtual void load(Level* level, float x, float y, float radius, sf::Color color, float brightness);

    virtual void update(sf::Time &gameTime);

    virtual void distributedUpdate(sf::Time &gameTime);

private:

    virtual void setVisible(int x, int y, float distance);

    virtual bool isBlocked(int x, int y);
};

#endif