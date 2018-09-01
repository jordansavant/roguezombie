#pragma once
#ifndef RZ_SKELETONLORD_H
#define RZ_SKELETONLORD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class SkeletonLord : public Character
{
public:

    SkeletonLord();

    virtual void load(Level* level, unsigned int id, float x, float y);

};

#endif