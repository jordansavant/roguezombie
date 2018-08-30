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

    schema.maxHealth = 15;
    schema.health = 15;

    Item* pistol = Item::create(Item::Biosprayer, level->server->getNextItemId());
    equip(Character::EquipmentSlot::WeaponPrimary, pistol);

}