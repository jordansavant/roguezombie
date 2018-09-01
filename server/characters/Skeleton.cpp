#include "Skeleton.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"
#include "../items/Item.hpp"

Skeleton::Skeleton()
    : Character()
{
    consumptionHeal = 0; // if eaten I will not heal
}

void Skeleton::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Skeleton, _x, _y, _level->tileWidth, _level->tileHeight);

    schema.maxHealth = 10;
    schema.health = 10;
    schema.speed = 2;
}