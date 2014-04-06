#pragma once
#ifndef RZ_ROGUEZOMBIESERVER_H
#define RZ_ROGUEZOMBIESERVER_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"

class RogueZombieServer: public bit::Game
{
public:
		RogueZombieServer();

        static unsigned int serverState;

		virtual void update(sf::Time &gameTime);

        virtual void registerStates();
};

#endif