#pragma once
#ifndef RZ_HAZMASTER_H
#define RZ_HAZMASTER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class DialogNode;

class Hazmaster : public Character
{
public:

    Hazmaster();

    virtual void load(Level* level, unsigned int id, float x, float y);

};

#endif