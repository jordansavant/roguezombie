#pragma once
#ifndef RZ_ROGUEZOMBIEGAME_H
#define RZ_ROGUEZOMBIEGAME_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"

class RogueZombieGame: public bit::Game
{
public:
		RogueZombieGame();

        static unsigned int introState;
        static unsigned int startMenuState;
        static unsigned int hostState;
        static unsigned int clientState;

		void update(sf::RenderWindow &window, sf::Time &gameTime);

		void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void registerStates();
};

#endif