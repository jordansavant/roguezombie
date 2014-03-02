#pragma once
#ifndef RZ_ZOMBIECLIENT_H
#define RZ_ZOMBIECLIENT_H

#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include <deque>

class WorldClient;

class ZombieClient : public Zombie
{
public:

    ZombieClient();

    WorldClient* world;
    sf::Sprite renderSprite;
    sf::Texture* renderTexture;
	sf::Clock temporaryClockVariable;
	std::deque<std::pair<sf::Time, Zombie::DeltaState>> deltas;

    void clientLoad(WorldClient* world, sf::Texture* renderTexture);

    void clientUpdate(sf::Time &gameTime);

    void clientDraw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif