#include "ClientZombie.hpp"
#include "Zombie.hpp"
#include "World.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Input/InputManager.hpp"
#include "../bitengine/Game/Game.hpp"
#include "../bitengine/Math/Math.hpp"
#include "MultiplayerState.hpp"

ClientZombie::ClientZombie()
    : Zombie()
{
}

void ClientZombie::clientLoad(sf::Texture* texture)
{
    renderTexture = texture;
    renderSprite.setTexture(*texture);
}

void ClientZombie::clientUpdate(sf::Time &gameTime)
{
	// delta timeline will have 0 to 3 snapshots
	switch(deltas.size())
	{
		case 1:
		{
			renderSprite.setPosition(deltaState.x, deltaState.y);
			break;
		}

		case 2:
		case 3:
		{	
			// interpolate from position at 0 to position at 1
			unsigned int msX = temporaryClockVariable.getElapsedTime().asMilliseconds() - 100;
			unsigned int msA = deltas[0].first.asMilliseconds();
			unsigned int msB = deltas[1].first.asMilliseconds();
			float xA = deltas[0].second.x;
			float xB = deltas[1].second.x;
			float yA = deltas[0].second.y;
			float yB = deltas[1].second.y;
			float flength = msB - msA;
			float ilength = msX - msA;
			float ratio = ilength / flength;
			float lerpX = bit::Math::lerp(xA, xB, ratio);
			float lerpY = bit::Math::lerp(yA, yB, ratio);

			renderSprite.setPosition(lerpX, lerpY);
		}
	}

}

void ClientZombie::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}

sf::Packet& ClientZombie::extractSnapshot(sf::Packet &packet)
{
	Zombie::extractSnapshot(packet);

	deltas.push_back(std::pair<sf::Time, DeltaState>(temporaryClockVariable.getElapsedTime(), deltaState));
	if(deltas.size() > 3)
	{
		deltas.pop_front();
	}

	return packet;
}