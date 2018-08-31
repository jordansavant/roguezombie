#pragma once
#ifndef RZ_CHARACTEREFFECT_H
#define RZ_CHARACTEREFFECT_H

class Character;

class CharacterEffect
{
public:

    CharacterEffect();

    enum Type
    {
        None,
        Poison
    };

    Type type;
    int damageAmount;
    unsigned int tileInterval;
    unsigned int maxTiles;
    unsigned int tileCounter;

    void run(Character* character);

    bool isComplete();

    static CharacterEffect* poison(int damageAmount, unsigned int tileInterval, unsigned int maxTiles);
};

#endif