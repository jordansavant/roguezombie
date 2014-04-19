#pragma once
#ifndef RZ_ITEM_H
#define RZ_ITEM_H

#include "ItemCategory.hpp"

class Item
{
public:

    Item();

    enum Type
    {
        Backpack,
        HardHat,
    };

    unsigned int CategoryBase;
    unsigned int CategoryArmor;
    unsigned int CategoryWeapon;
    unsigned int CategoryJewelry;
    unsigned int CategoryContainer;

    bool isOf(unsigned int currentValue, unsigned int filter);

    bool isExactly(unsigned int currentValue, unsigned int filter);

    static Item* create(Type type);
};

#endif