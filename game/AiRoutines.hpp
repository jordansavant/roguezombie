#pragma once
#ifndef RZ_AIROUTINES_H
#define RZ_AIROUTINES_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"

class Character;
class Structure;
class Tile;
class Mission;
class Item;

class AiRoutines
{
public:

    class Combat
    {
    public:

        static void DecideZombie(Character* character);

    };
};

#endif