#pragma once
#ifndef RZ_SKELETON_H
#define RZ_SKELETON_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class Skeleton : public Character
{
public:

    Skeleton();

    virtual void load(Level* level, unsigned int id, float x, float y);

};

#endif