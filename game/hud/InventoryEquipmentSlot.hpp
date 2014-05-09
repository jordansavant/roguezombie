#pragma once
#ifndef RZ_INVENTORYEQUIPMENTSLOT_H
#define RZ_INVENTORYEQUIPMENTSLOT_H

#include "Inventory.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"
#include "../Character.hpp"

class InventoryItemLabel;

class InventoryEquipmentSlot : public bit::Container
{
public:

    InventoryEquipmentSlot(Character::EquipmentSlot slot, Inventory* inventory, float relativeX, float relativeY, float width, float height, AnchorType anchorType);

    Inventory* inventory;
    InventoryItemLabel* equippedItemLabel;
    Character::EquipmentSlot slot;

    void addItemLabel(InventoryItemLabel* label);

    void removeItemLabel();

    virtual unsigned int addChild(Element* child);

    virtual void moveChild(Container* other, unsigned int childIndex);

    virtual void removeChild(unsigned int childIndex);
};

#endif