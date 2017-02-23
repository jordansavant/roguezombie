#pragma once
#ifndef RZ_LEVELRUNNABLE_H
#define RZ_LEVELRUNNABLE_H

#include "../bitengine/Game.hpp"

class LevelRunnable
{
public:

    LevelRunnable();

    bool removeFromLevel;

    virtual void update(sf::Time &gameTime) = 0;

    virtual void distributedUpdate(sf::Time &gameTime) = 0;

};

#endif