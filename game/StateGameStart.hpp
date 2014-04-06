#pragma once
#ifndef RZ_STARTMENUSTATE_H
#define RZ_STARTMENUSTATE_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"

namespace bit
{
    class StateStack;
    class Camera;
    class Game;
}

class RogueZombieGame;

class StateGameStart : public bit::State
{
public:

    StateGameStart(bit::StateStack &stack, RogueZombieGame* game);

    RogueZombieGame* rogueZombieGame;
	sf::CircleShape shape;

    virtual bool handleInput(sf::RenderWindow &window, sf::Time &gameTime);

    virtual bool update(sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);
};

#endif
