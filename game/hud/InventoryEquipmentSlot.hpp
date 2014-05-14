#pragma once
#ifndef RZ_INVENTORYEQUIPMENTSLOT_H
#define RZ_INVENTORYEQUIPMENTSLOT_H

#include "Inventory.hpp"
#include "Frame.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"
#include "../Character.hpp"

class Hud;
class InventoryItemLabel;

class InventoryEquipmentSlot : public Frame
{
public:

    InventoryEquipmentSlot(Hud* hud, Character::EquipmentSlot slot, Inventory* inventory, float relativeX, float relativeY, float width, float height, AnchorType anchorType);

    Inventory* inventory;
    InventoryItemLabel* equippedItemLabel;
    Character::EquipmentSlot slot;
    unsigned int AcceptedCategoryArmor;
    unsigned int AcceptedCategoryWeapon;
    unsigned int AcceptedCategoryJewelry;

    void addItemLabel(InventoryItemLabel* label);

    void removeItemLabel();

    bool acceptsLabel(InventoryItemLabel* label);

    virtual unsigned int addChild(Element* child);

    virtual void moveChild(Container* other, unsigned int childIndex);

    virtual void moveChild(Container* other, Element* child);

    virtual void removeChild(unsigned int childIndex);
};

#endif