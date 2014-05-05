#pragma once
#ifndef RZ_INVENTORYEQUIPMENTSLOT_H
#define RZ_INVENTORYEQUIPMENTSLOT_H

#include "Inventory.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"

class InventoryItemLabel;

class InventoryEquipmentSlot : public bit::Container
{
public:

    InventoryEquipmentSlot(Inventory* inventory, float relativeX, float relativeY, float width, float height, AnchorType anchorType);

    Inventory* inventory;
    InventoryItemLabel* equippedItemLabel;

    void addItemLabel(InventoryItemLabel* label);

    void removeItemLabel();

    virtual unsigned int addChild(Element* child);

    virtual void moveChild(Container* other, unsigned int childIndex);

};

#endif