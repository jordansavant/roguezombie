#pragma once
#ifndef TEST_GAME_H
#define TEST_GAME_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game/Game.hpp"

class TestGame: public bit::Game
{
public:
		TestGame();

        static unsigned int titleState;
        static unsigned int hostState;
        static unsigned int clientState;

		void update(sf::RenderWindow &window, sf::Time &gameTime);

		void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void registerStates();
};

#endif