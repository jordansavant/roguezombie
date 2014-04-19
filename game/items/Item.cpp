#include "Item.hpp"
#include "ItemCategory.hpp"
#include "../../bitengine/Math.hpp"

Item::Item()
    : CategoryBase(0), CategoryArmor(0), CategoryWeapon(0), CategoryJewelry(0), CategoryContainer(0), weight(0)
{
}

bool Item::hasAny(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAny(attribute, filter);
}

bool Item::hasAll(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAll(attribute, filter);
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
            i->CategoryArmor = ItemCategory::Weapon::WeaponRanged;
            i->weight = 2;

            break;
    }

    return i;
}