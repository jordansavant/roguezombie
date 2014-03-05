#include "OgreClient.hpp"
#include "Ogre.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../GameplayState.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

OgreClient::OgreClient()
    : Ogre(), renderX(0), renderY(0)
{
}

void OgreClient::clientLoad(WorldClient* _world, sf::Texture* texture)
{
    world = _world;
    renderTexture = texture;
    renderSprite.setTexture(*texture);
}

void OgreClient::clientUpdate(sf::Time &gameTime)
{
	float finalX = Body::deltaState.x;
	float finalY = Body::deltaState.y;

	// delta timeline will have 0 to 3 snapshots
	if(bodyDeltas.size() > 1)
	{
		int ticktime = (1000 / BIT_SERVER_TICK_FPS);
		int lagtime = 2 * ticktime;

		// interpolate from position at 0 to position at 1
		int a = 0;
		int b = 1;

		// Set point of render time 100ms behind server snapshots
		int msX = temporaryClockVariable.getElapsedTime().asMilliseconds() - lagtime;

		// Get the first delta and the second delta, this should theoretically wrap
		// the execution snapshot if there were not dropped packets
		int msA = bodyDeltas[a].first.asMilliseconds();
		int msB = bodyDeltas[b].first.asMilliseconds();
        msX = std::max(0, msX);
        msA = std::max(0, msA);

		// If the target render point is later than our second packet, try our third
		if(msX > msB && bodyDeltas.size() == 3)
		{
			b = 2;
			msB = bodyDeltas[b].first.asMilliseconds();
		}
        msB = std::max(0, msB);

		//  If the render point has been wrapped by two known histories
		if(msX <= msB)
		{
			// Interpolate

			// Get the wrapped positions
			float xA = bodyDeltas[a].second.x;
			float xB = bodyDeltas[b].second.x;
			float yA = bodyDeltas[a].second.y;
			float yB = bodyDeltas[b].second.y;

			// Get their ratio of time
			int flength = msB - msA;
			int ilength = msX - msA;
			float ratio = (float)ilength / (float)flength;

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

            if(bodyDeltas.size() == 3)
            {
                indexA = 1;
                indexB = 2;
            }

            float positionOneX = bodyDeltas[indexA].second.x;
            float positionOneY = bodyDeltas[indexA].second.y;
            int positionOneTime = bodyDeltas[indexA].first.asMilliseconds();

            float positionTwoX = bodyDeltas[indexB].second.x;
            float positionTwoY = bodyDeltas[indexB].second.y;
            int positionTwoTime = bodyDeltas[indexB].first.asMilliseconds();

            int currentTime = temporaryClockVariable.getElapsedTime().asMilliseconds();

            float xDiff = positionTwoX - positionOneX;
            float yDiff = positionTwoY - positionOneY;
            int tDiff = positionTwoTime - positionOneTime;

            int currentDiff = currentTime - positionTwoTime;
            float ratio = (float)currentDiff / (float)tDiff;

            finalX = positionTwoX + ratio * ( xDiff );
            finalY = positionTwoY + ratio * ( yDiff );
		}
	}

    float spriteWidth = 84;
    float spriteHeight = 116;
    float xFootOffset = 30;
    float yFootOffset = 20;

    float worldX = finalX;
    float worldY = finalY;
    float worldCenterX = worldX + Body::deltaState.width / 2;
    float worldCenterY = worldY + Body::deltaState.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(worldCenterX, worldCenterY);
    renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    renderY = renderPosition.y - spriteHeight + yFootOffset;
    renderSprite.setPosition(renderX, renderY);
}

void OgreClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}

void OgreClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	Ogre::handleSnapshot(packet, full);

	bodyDeltas.push_back(std::pair<sf::Time, Body::DeltaState>(temporaryClockVariable.getElapsedTime(), Body::deltaState));
	characterDeltas.push_back(std::pair<sf::Time, Character::DeltaState>(temporaryClockVariable.getElapsedTime(), Character::deltaState));

	if(bodyDeltas.size() > 3)
	{
		bodyDeltas.pop_front();
		characterDeltas.pop_front();
	}
}