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

        static bool Zombie_DetectHostility(Character* character, Character* other);

        static bool Zombie_DetectCombat(Character* character);

        static void Zombie_DecideCombat(Character* character);

        static bool Hunter_DetectHostility(Character* character, Character* other);

        static void Hunter_DecideCombat(Character* character);

    };
};

#endif