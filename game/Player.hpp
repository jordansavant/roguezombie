#pragma once
#ifndef RZ_PLAYER_H
#define RZ_PLAYER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"

class Level;
class Character;

class Player
{
public:

    Player();

    Level* level;
	Character* character;
    unsigned int clientId;

    void load(unsigned int clientId);

    void setLevel(Level* level);

    void setCharacter(Character* character);
};

#endif