#pragma once
#ifndef BIT_GAMECOMPONENT_H
#define BIT_GAMECOMPONENT_H

#include "SFML/Graphics.hpp"

namespace bit
{
	class GameComponent
	{
	public:
        virtual void update(sf::RenderWindow &window, sf::Time &gameTime, bool isGamePaused);
        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime, bool isGamePaused);
	};
}

#endif