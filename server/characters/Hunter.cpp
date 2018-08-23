#include "Hunter.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"
#include "../items/Item.hpp"

Hunter::Hunter()
    : Character()
{
    consumptionHeal = 10; // if eaten I will heal
}

void Hunter::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Hunter, _x, _y, _level->tileWidth, _level->tileHeight);

    schema.maxHealth = 35;
    schema.health = 35;

    Item* rifle = Item::create(Item::Z4Rifle, level->server->getNextItemId());
    equip(Character::EquipmentSlot::WeaponPrimary, rifle);
}

void Hunter::update(sf::Time &gameTime)
{
    Character::update(gameTime);
}