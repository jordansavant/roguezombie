#pragma once
#ifndef BIT_GAME_H
#define BIT_GAME_H

#include "SFML/Graphics.hpp"
#include "../System/PlatformDefinitions.hpp"

#if defined(__OS_WIN__)
#if defined(_DEBUG)
#include "vld.h"
#endif
#endif

namespace bit
{
    class GameComponent;
    class StateStack;

	class Game
	{
	public:
 		Game();

        virtual ~Game();

        StateStack* stateStack;
		std::vector<GameComponent*> gameComponents;
		sf::Clock clock;
		float FPS;

		void run();

		virtual void update(sf::Time &gameTime);

        virtual void registerStates();

        void quit();

	protected:
		float actualTime;
		float dt;
		float timer;
        bool running;
	};
}

#endif