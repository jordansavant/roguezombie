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

class StartMenuState : public bit::State
{
public:

    StartMenuState(bit::StateStack &stack, bit::Game* game);

	sf::CircleShape shape;

    virtual bool handleInput(sf::RenderWindow &window, sf::Time &gameTime);

    virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);
};

#endif
