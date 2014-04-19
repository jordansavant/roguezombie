#pragma once
#ifndef RZ_ITEMCATEGORY_H
#define RZ_ITEMCATEGORY_H

class ItemCategory
{
public:

    enum Base
    {
        BaseNone = 0,
        BaseArmor = 2,
        BaseWeapon = 4,
        BaseJewelry = 8,
        BaseContainer = 16,
        //Consumable = 8,
        //Part = 16,
        //Junk = 32,
    };

    enum Armor
    {
        ArmorNone = 0,
        ArmorHead = 2,
        ArmorChest = 4,
        ArmorLeg = 8,
        ArmorShoulder = 16,
        ArmorHand = 32,
        ArmorFoot = 64,
    };

    enum Weapon
    {
        WeaponNone = 0,
        WeaponMelee = 2,
        WeaponRanged = 4,
    };

    enum Jewelry
    {
        JewelryNone = 0,
        JewelryTotem = 2,
        JewelryRing = 4,
    };

    enum Container
    {
        ContainerNone = 0,
        ContainerBackpack = 0,
    };
};

#endif