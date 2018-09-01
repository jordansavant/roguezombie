#pragma once
#ifndef RZ_AIROUTINES_H
#define RZ_AIROUTINES_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include <functional>

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

        static Tile* findCoverPositionFromEnemy(Character* character, Character* enemy, unsigned int tileRadius);

        static bool Default_DetectCombat(Character* character);

        static bool Zombie_DetectHostility(Character* character, Character* other);

        static void Zombie_DecideCombat(Character* character);

        static bool Generic_DetectHostility(Character* character, Character* other);

        static void Hunter_DecideCombat(Character* character);

        static void Guard_DecideCombat(Character* character);

        static void Scientist_DecideCombat(Character* character);

        static void Savage_DecideCombat(Character* character);

    private:
        
        static void findOpenAdjacentPosition(Character* character, Character* enemy, std::function<void(Character* character, Character* enemy, sf::Vector2i pos)> onSuccess, std::function<void(Character* character, Character* enemy)> onFailure);

    };
};

#endif