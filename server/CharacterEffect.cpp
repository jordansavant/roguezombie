#include "CharacterEffect.hpp"
#include "Character.hpp"
#include "../bitengine/System.hpp"

CharacterEffect::CharacterEffect()
    : type(Type::None), maxTiles(0), tileInterval(0), tileCounter(0)
{
}

void CharacterEffect::run(Character* character)
{
    switch (type)
    {
        case Type::Poison:
        {
            if (tileCounter % tileInterval == 0)
            {
                bit::Output::Debug("POISON DAMAGE");
                character->harm(damageAmount);
            }
            break;
        }
    }

    tileCounter++;
}

bool CharacterEffect::isComplete()
{
    return tileCounter >= maxTiles;
}

CharacterEffect* CharacterEffect::poison(int _damageAmount, unsigned int _tileInterval, unsigned int _maxTiles)
{
    CharacterEffect* e = new CharacterEffect();
    e->type = Type::Poison;
    e->damageAmount = _damageAmount;
    e->tileInterval = _tileInterval;
    e->maxTiles = _maxTiles;

    return e;
}