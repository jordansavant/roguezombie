#pragma once
#ifndef BIT_GAMETIMER_H
#define BIT_GAMETIMER_H

#include "SFML/System.hpp"

namespace bit
{
	class GameTimer
	{
	public:
		GameTimer();

		GameTimer(float seconds, bool beginMaxed = false);

		bool update(sf::Time &gameTime);

		void reset();

		void max();

		void adjustBy(float addSeconds);

		void setNewDuration(float newSeconds);

		float getCompletionRatio();

	private:
		float seconds;
		sf::Time waitTime;
		sf::Time counter;
	};
}

#endif