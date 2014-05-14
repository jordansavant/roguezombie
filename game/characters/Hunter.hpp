#pragma once
#ifndef RZ_HUNTER_H
#define RZ_HUNTER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class Hunter : public Character
{
public:

    Hunter();

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);

    virtual void prepareInteractionTree(bit::ServerPacket &packet);
};

#endif