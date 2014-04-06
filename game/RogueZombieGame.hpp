#pragma once
#ifndef RZ_ROGUEZOMBIEGAME_H
#define RZ_ROGUEZOMBIEGAME_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"

class RogueZombieGame: public bit::VideoGame
{
public:
		RogueZombieGame();

        static unsigned int stateGameIntroduction;
        static unsigned int stateGameStart;
        static unsigned int stateGamePlayHost;
        static unsigned int stateGamePlayClient;

        bit::FrameTimer fps;

		virtual void update(sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void registerStates();
};

#endif