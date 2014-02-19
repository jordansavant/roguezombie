#pragma once
#ifndef RZ_ZOMBIE_H
#define RZ_ZOMBIE_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game/GameTimer.hpp"

class MultiplayerState;

class Zombie
{
public:

    Zombie();

    float x;
    float y;
    sf::Vector2f direction;
    float centerX;
    float centerY;
    int health;
    int maxHealth;
    bool isPlayer;
    MultiplayerState* state;
    sf::Sprite zombiesprite;
    sf::Texture* zombietexture;
    bit::GameTimer walkTimer;

    void load(MultiplayerState* state, sf::Texture &texture, float x, float y);

    void update(sf::RenderWindow &window, sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

};

#endif