#pragma once
#ifndef RZ_ZOMBIE_H
#define RZ_ZOMBIE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../Character.hpp"

class Zombie : public Character
{
public:

    Zombie();

    bit::GameTimer walkTimer;
	bool isPlayerControlled;

    virtual void load(World* world, Tile* tile);

    virtual void update(sf::Time &gameTime);

	void updatePosition(sf::Vector2f &direction);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif