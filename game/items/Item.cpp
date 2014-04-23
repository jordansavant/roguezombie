#include "Item.hpp"
#include "ItemCategory.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/Network.hpp"
#include "../GameEvent.hpp"
#include "../Character.hpp"
#include "../Player.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"

Item::Item()
    : CategoryBase(0), CategoryArmor(0), CategoryWeapon(0), CategoryJewelry(0), CategoryContainer(0),
      id(0), parentItem(NULL), parentCharacter(NULL), type(Type::None), weight(0), canContainItems(false), itemLimit(0)
{
}

Item::~Item()
{
    for(unsigned int i=0; i < items.size(); i++)
    {
        delete items[i];
    }
}

bool Item::hasAny(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAny(attribute, filter);
}

bool Item::hasAll(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAll(attribute, filter);
}

void Item::addItem(Item* item)
{
    if(canContainItems && items.size() + 1 <= itemLimit)
    {
        items.push_back(item);
        item->parentItem = this;
        item->sendAddItemPacket();
    }
}

Character* Item::getParentCharacter()
{
    if(parentItem)
    {
        return parentItem->getParentCharacter();
    }

    return parentCharacter;
}

// First elements are highest parents
void Item::fillIdHierarchy(std::vector<unsigned int> &fill)
{
    if(parentItem)
    {
        parentItem->fillIdHierarchy(fill);
    }
    fill.push_back(id);
}

void Item::packIdHierarchy(bit::ServerPacket &packet)
{
    std::vector<unsigned int> parents;
    fillIdHierarchy(parents);

    packet << sf::Uint32(parents.size());
    for(unsigned int i=0; i < parents.size(); i++)
    {
        packet << sf::Uint32(parents[i]);
    }
}

void Item::sendAddItemPacket()
{
    Character* c = getParentCharacter();
    if(c->schema.isPlayerCharacter)
    {
        Item* i = this;
        c->level->server->sendEventToClient(*c->schema.player->client, [i] (bit::ServerPacket &packet) {

            packet << sf::Uint32(GameEvent::ItemAdded);
            i->packIdHierarchy(packet);
            i->prepareSnapshot(packet);
        });
    }
}

void Item::prepareSnapshot(bit::ServerPacket &packet)
{
    packet << sf::Uint32(CategoryBase);
    packet << sf::Uint32(CategoryArmor);
    packet << sf::Uint32(CategoryWeapon);
    packet << sf::Uint32(CategoryJewelry);
    packet << sf::Uint32(CategoryContainer);

    packet << sf::Uint32(type);
    packet << weight;

    packet << sf::Uint32(items.size());
    for(unsigned int i=0; i < items.size(); i++)
    {
        packet << sf::Uint32(items[i]->id);
        items[i]->prepareSnapshot(packet);
    }
}

std::string Item::getTitle(Type type)
{
    switch(type)
    {
        default:
        case Type::None:
            return "None";

        case Type::Backpack:
            return "Backpack";

        case Type::HardHat:
            return "Hardhat";
                
        case Type::Magnum357:
            return ".357 Magnum";
    }
}

Item* Item::create(Type type)
{
    Item* i = 0;

    switch(type)
    {
        case Type::Backpack:

            i = new Item();
            i->CategoryBase = ItemCategory::Base::BaseContainer;
            i->CategoryContainer = ItemCategory::Container::ContainerBackpack;
            i->weight = 0;
            i->canContainItems = true;
            i->itemLimit = 1000;

            break;
        
        case Type::HardHat:

            i = new Item();
            i->CategoryBase = ItemCategory::Base::BaseArmor;
            i->CategoryArmor = ItemCategory::Armor::ArmorHead;
            i->weight = 1.5;

            break;
                
        case Type::Magnum357:

            i = new Item();
            i->CategoryBase = ItemCategory::Base::BaseWeapon;
            i->CategoryWeapon = ItemCategory::Weapon::WeaponRanged;
            i->weight = 2;

            break;
    }

    i->type = type;
    return i;
}