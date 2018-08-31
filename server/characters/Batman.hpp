#pragma once
#ifndef RZ_BATMAN_H
#define RZ_BATMAN_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class Batman : public Character
{
public:

    Batman();

    virtual void load(Level* level, unsigned int id, float x, float y);

};

#endif