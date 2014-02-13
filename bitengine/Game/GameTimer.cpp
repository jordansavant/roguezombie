#include "GameTimer.hpp"

bit::GameTimer::GameTimer()
	: waitTime(), counter()
{
	seconds = 0;
}

bit::GameTimer::GameTimer(float seconds, bool beginMaxed)
{
	setNewDuration(seconds);

	if(beginMaxed)
	{
		counter += sf::seconds(seconds);
	}
}

bool bit::GameTimer::update(sf::Time &gameTime)
{
	counter += gameTime;

	float cs = counter.asSeconds();
	float ws = waitTime.asSeconds();
	if(cs >= ws)
    {
		counter = sf::seconds(0);
        return true;
    }

    return false;
}

void bit::GameTimer::reset()
{
	counter = sf::seconds(0);
}

void bit::GameTimer::max()
{
	counter = waitTime;
}

void bit::GameTimer::adjustBy(float addSeconds)
{
	waitTime = sf::seconds(seconds + addSeconds);
}

void bit::GameTimer::setNewDuration(float newSeconds)
{
	seconds = newSeconds;
	counter = sf::seconds(0);
	waitTime = sf::seconds(seconds);
}

float bit::GameTimer::getCompletionRatio()
{
	return ((float)counter.asMilliseconds() / (float)waitTime.asMilliseconds());
}