#pragma once
#ifndef RZ_CLIENTZOMBIE_H
#define RZ_CLIENTZOMBIE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game/GameTimer.hpp"
#include "Zombie.hpp"
#include <deque>

namespace bit
{
    class ServerPacket;
}

class World;

class ClientZombie : public Zombie
{
public:

    ClientZombie();

    sf::Sprite renderSprite;
    sf::Texture* renderTexture;
	sf::Clock temporaryClockVariable;
	std::deque<std::pair<sf::Time, Zombie::DeltaState>> deltas;

    void clientLoad(sf::Texture* renderTexture);

    void clientUpdate(sf::Time &gameTime);

    void clientDraw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void handleSnapshot(bit::ServerPacket &packet);

};

#endif