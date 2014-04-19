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
        Magnum357,
    };

    unsigned int CategoryBase;
    unsigned int CategoryArmor;
    unsigned int CategoryWeapon;
    unsigned int CategoryJewelry;
    unsigned int CategoryContainer;

    float weight;

    bool hasAny(unsigned int currentValue, unsigned int filter);

    bool hasAll(unsigned int currentValue, unsigned int filter);

    static Item* create(Type type);
};

#endif