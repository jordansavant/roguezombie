#include "Hazmaster.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"
#include "../items/Item.hpp"

Hazmaster::Hazmaster()
    : Character()
{
    consumptionHeal = 10; // if eaten I will heal
}

void Hazmaster::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Hazmaster, _x, _y, _level->tileWidth, _level->tileHeight);

    schema.maxHealth = 20;
    schema.health = 20;

    if (bit::Math::random(2) == 0) {
        Item* pistol = Item::create(Item::Magnum357, level->server->getNextItemId());
        equip(Character::EquipmentSlot::WeaponPrimary, pistol);
    }
    else {
        Item* baton = Item::create(Item::Baton, level->server->getNextItemId());
        equip(Character::EquipmentSlot::WeaponPrimary, baton);
    }
}