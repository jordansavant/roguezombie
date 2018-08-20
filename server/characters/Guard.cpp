#include "Guard.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"
#include "../items/Item.hpp"

Guard::Guard()
    : Character()
{
}

void Guard::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Guard, _x, _y, _level->tileWidth, _level->tileHeight);

    schema.maxHealth = 20;
    schema.health = 20;

    Item* pistol = Item::create(Item::Magnum357, level->server->getNextItemId());
    equip(Character::EquipmentSlot::WeaponPrimary, pistol);
}

void Guard::update(sf::Time &gameTime)
{
    Character::update(gameTime);
}