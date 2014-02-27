#pragma once
#ifndef RZ_ZOMBIE_H
#define RZ_ZOMBIE_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game/GameTimer.hpp"

class World;

class Zombie
{
public:

    Zombie();

    // Server
    float x;
    float y;
    sf::Vector2f direction;
    int health;
    int maxHealth;
    bit::GameTimer walkTimer;
    World* world;
	bool isPlayerControlled;

    void load(World* world, float x, float y);

    void update(sf::Time &gameTime);

	void updatePosition(sf::Vector2f &direction);

    sf::Packet& compileSnapshot(sf::Packet& packet);

    sf::Packet& extractSnapshot(sf::Packet& packet);

};

#endif