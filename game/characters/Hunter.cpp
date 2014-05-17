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
#include "../dialog/DialogEntry.hpp"
#include "../dialog/DialogNode.hpp"

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

DialogNode* Hunter::getDefaultDialogNode()
{
    return DialogEntry::getDialogTree();
}

void Hunter::handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket)
{
    Character::handleInteraction(interaction, interactor, responsePacket);

    switch(interaction)
    {
        case Interaction::Type::Dialog:
        {
            interactor->conversationSpeaker = this;
            prepareDialogNode(interactor, responsePacket);
            break;
        }
    }
}

void Hunter::getAvailableInteractions(std::vector<Interaction::Type> &fill)
{
    Character::getAvailableInteractions(fill);

    fill.push_back(Interaction::Type::Dialog);
}