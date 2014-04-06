#pragma once
#ifndef BIT_GAMECOMPONENT_H
#define BIT_GAMECOMPONENT_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class Game;

	class GameComponent
	{
	public:

        GameComponent(Game* game);

        virtual ~GameComponent();

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        Game* game;
	};
}

#endif