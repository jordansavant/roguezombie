#pragma once
#ifndef RZ_RPGSYSTEM_H
#define RZ_RPGSYSTEM_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"

class Character;
class Structure;
class Tile;
class Mission;
class Item;

class RpgSystem
{
public:

    static int masterIntelligence;
    static int masterSpeed;
    static int masterDexterity;
    static int masterStrength;

    static float calculateDexterityFactor(Character* character);

    static float calculateStrengthFactor(Character* character);

    class Combat
    {
    public:

        static float calculateChanceOfHit(Character* attacker, Character* defender);

        static float calculateRangedChanceOfHit(Item* weapon, Character* attacker, Character* defender);

        static float calculateRangedObstructionPenalty(Character* attacker, Character* defender);

        static float calculateRangedDistanceFactor(Item* weapon, Character* attacker, Character* defender);

        static float calculateMeleeChanceOfHit(Item* weapon, Character* attacker, Character* defender);

        static float calculateUnarmedChanceOfHit(Character* attacker, Character* defender);

        static float calculateCoreMeleeChanceOfHit(Character* attacker, Character* defender);

        static float calculateDodgeBypass(Character* attacker);

        static float calculateBlockBypass(Character* attacker);

        static float calculateDodgeRating(Character* defender);

        static float calculateBlockRating(Character* defender);

    };

};

#endif