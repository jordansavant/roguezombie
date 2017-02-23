#pragma once
#ifndef RZ_STATEGAMEERROR_H
#define RZ_STATEGAMEERROR_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Gui.hpp"

class RogueZombieGame;
class ErrorMenu;

class StateGameError : public bit::State
{
public:

    StateGameError(bit::StateStack &stack, RogueZombieGame* game);

    virtual ~StateGameError();

    RogueZombieGame* rogueZombieGame;
    ErrorMenu* errorMenu;

    virtual bool update(sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);
};

#endif