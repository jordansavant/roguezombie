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
    unsigned int id;
    float x;
    float y;
    sf::Vector2f direction;
    int health;
    int maxHealth;
    bit::GameTimer walkTimer;
    World* world;

    // Client
    sf::Sprite renderSprite;
    sf::Texture* renderTexture;

    void clientLoad(sf::Texture* renderTexture);

    void serverLoad(unsigned int id, World* world, float x, float y);

    void clientUpdate(sf::Time &gameTime);

    void serverUpdate(sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    sf::Packet& compileSnapshot(sf::Packet& packet);

    sf::Packet& extractSnapshot(sf::Packet& packet);

};

#endif