#include "Item.hpp"
#include "ItemCategory.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/Network.hpp"

Item::Item()
    : CategoryBase(0), CategoryArmor(0), CategoryWeapon(0), CategoryJewelry(0), CategoryContainer(0),
      id(0), type(Type::None), weight(0), canContainItems(false), itemLimit(0)
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
    packet << sf::Uint32(weight);

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