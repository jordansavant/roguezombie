#include "Item.hpp"
#include "ItemCategory.hpp"

Item::Item()
    : CategoryBase(0), CategoryArmor(0), CategoryWeapon(0), CategoryJewelry(0), CategoryContainer(0)
{
}

bool Item::isExactly(unsigned int attribute, unsigned int filter)
{
    return (attribute & filter) == filter;
}

bool Item::isOf(unsigned int attribute, unsigned int filter)
{
    return (attribute & filter) > 0;
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

            break;
        
        case Type::HardHat:

            i = new Item();
            i->CategoryBase = ItemCategory::Base::BaseArmor;
            i->CategoryArmor = ItemCategory::Armor::ArmorHead | ItemCategory::Armor::ArmorHand;

            break;
    }

    return i;
}