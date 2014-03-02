#include "ZombieClient.hpp"
#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"

ZombieClient::ZombieClient()
    : Zombie()
{
}

void ZombieClient::clientLoad(WorldClient* _world, sf::Texture* texture)
{
    world = _world;
    renderTexture = texture;
    renderSprite.setTexture(*texture);
}

void ZombieClient::clientUpdate(sf::Time &gameTime)
{
	float finalX = deltaState.x;
	float finalY = deltaState.y;

	// delta timeline will have 0 to 3 snapshots
	if(deltas.size() > 1)
	{
		unsigned int ticktime = (1000 / BIT_SERVER_TICK_FPS);
		unsigned int lagtime = 2 * ticktime;

		// interpolate from position at 0 to position at 1
		unsigned int a = 0;
		unsigned int b = 1;

		// Set point of render time 100ms behind server snapshots
		unsigned int msX = temporaryClockVariable.getElapsedTime().asMilliseconds() - lagtime;

		// Get the first delta and the second delta, this should theoretically wrap
		// the execution snapshot if there were not dropped packets
		unsigned int msA = deltas[a].first.asMilliseconds();
		unsigned int msB = deltas[b].first.asMilliseconds();

		// If the target render point is later than our second packet, try our third
		/*if(msX > msB && deltas.size() == 3)
		{
			b = 2;
			msB = deltas[b].first.asMilliseconds();
		}*/

		//  If the render point has been wrapped by two known histories
		if(msX <= msB)
		{
			// Interpolate

			// Get the wrapped positions
			float xA = deltas[a].second.x;
			float xB = deltas[b].second.x;
			float yA = deltas[a].second.y;
			float yB = deltas[b].second.y;

			// Get their ratio of time
			float flength = (float)msB - (float)msA;
			float ilength = (float)msX - (float)msA;
			float ratio = ilength / flength;

			// Lerp them
			float lerpX = bit::Math::lerp(xA, xB, ratio);
			float lerpY = bit::Math::lerp(yA, yB, ratio);

			finalX = lerpX;
			finalY = lerpY;
		}
		// If the render point has lost too many packets but not for too long
		else if(msX - msB < ticktime * 5)
		{
			// Extrapolate - TODO
            int indexA = 0;
            int indexB = 1;

            if(deltas.size() == 3)
            {
                indexA = 1;
                indexB = 2;
            }

            float positionOneX = deltas[indexA].second.x;
            float positionOneY = deltas[indexA].second.y;
            float positionOneTime = deltas[indexA].first.asMilliseconds();

            float positionTwoX = deltas[indexB].second.x;
            float positionTwoY = deltas[indexB].second.y;
            float positionTwoTime = deltas[indexB].first.asMilliseconds();

            float currentTime = temporaryClockVariable.getElapsedTime().asMilliseconds();

            float xDiff = positionTwoX - positionOneX;
            float yDiff = positionTwoY - positionOneY;
            float tDiff = positionTwoTime - positionOneTime;

            float currentDiff = currentTime - positionTwoTime;
            float ratio = currentDiff / tDiff;

            finalX = positionTwoX + ratio * ( xDiff );
            finalY = positionTwoY + ratio * ( yDiff );
		}
		// If the render point is behind for more than 1/4 of a second, do no prediction
        else
        {
            finalX = 10;
            finalY = 10;
        }
	}

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(finalX, finalY);
    renderSprite.setPosition(renderPosition.x, renderPosition.y);
}

void ZombieClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}

void ZombieClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	Zombie::handleSnapshot(packet, full);

	deltas.push_back(std::pair<sf::Time, DeltaState>(temporaryClockVariable.getElapsedTime(), deltaState));
	if(deltas.size() > 3)
	{
		deltas.pop_front();
	}
}