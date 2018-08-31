#include "Batman.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"
#include "../items/Item.hpp"

Batman::Batman()
    : Character()
{
    consumptionHeal = 10; // if eaten I will heal
}

void Batman::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Batman, _x, _y, _level->tileWidth, _level->tileHeight);

    schema.maxHealth = 100;
    schema.health = 100;
    schema.dexterity = 3;
    schema.strength = 4;
}