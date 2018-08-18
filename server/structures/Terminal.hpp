#pragma once
#ifndef RZ_TERMINAL_H
#define RZ_TERMINAL_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../Structure.hpp"

class Level;
class Character;

class Terminal : public Structure
{
public:

    Terminal();

    struct Schema
    {
        Schema()
        {
        }

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            return packet;
        }
    };
    Schema schema;

    virtual void load(Level* level, unsigned int id, float x, float y);

    virtual void update(sf::Time &gameTime);
};

#endif