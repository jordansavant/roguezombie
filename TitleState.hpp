#pragma once
#ifndef TITLESTATE_H
#define TITLESTATE_H

#include "bitengine/Game/State.hpp"
#include "SFML/Graphics.hpp"

namespace bit
{
    class StateStack;
    class Camera;
}

class TitleState : public bit::State
{
public:

    TitleState(bit::StateStack &stack);
    
    sf::Texture zombieimage;
    sf::Sprite zombiesprite;
	sf::CircleShape shape;
    
    virtual bool handleInput(sf::RenderWindow &window, sf::Time &gameTime);

    virtual bool update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);

};

#endif