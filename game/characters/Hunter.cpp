#include "Hunter.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"
#include "../GameplayServer.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../items/Item.hpp"

Hunter::Hunter()
    : Character()
{
}

void Hunter::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Hunter, _x, _y, _level->tileWidth, _level->tileHeight);

    Item* rifle = Item::create(Item::Z4Rifle, level->server->getNextItemId());
    equip(Character::EquipmentSlot::WeaponPrimary, rifle);
}

void Hunter::update(sf::Time &gameTime)
{
    Character::update(gameTime);
}

void Hunter::prepareInteractionTree(bit::ServerPacket &packet)
{
    // Pack tile id
    packet << sf::Uint32(1);
    packet << sf::Uint32(Interaction::Type::Converse);
}