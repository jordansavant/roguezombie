#include "SkeletonLord.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"
#include "../items/Item.hpp"

SkeletonLord::SkeletonLord()
    : Character()
{
    consumptionHeal = 0; // if eaten I will not heal
}

void SkeletonLord::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::SkeletonLord, _x, _y, _level->tileWidth, _level->tileHeight);

    schema.maxHealth = 30;
    schema.health = 30;
    schema.speed = 3;

    Item* weapon = Item::create(Item::Wand, level->server->getNextItemId());
    equip(Character::EquipmentSlot::WeaponPrimary, weapon);
}