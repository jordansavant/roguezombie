#pragma once
#ifndef RZ_INVENTORYITEMLABEL_H
#define RZ_INVENTORYITEMLABEL_H

#include "Inventory.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"

class InventoryEquipmentSlot;
class InventoryPositionSlot;

class InventoryItemLabel : public bit::Label
{
public:

    InventoryItemLabel(Inventory* inventory, ItemClient* item, float relativeX, float relativeY, AnchorType anchorType);

    Inventory* inventory;
    Item::Schema itemSchema;
    InventoryEquipmentSlot* currentEquipmentSlot;
    InventoryPositionSlot* currentPositionSlot;

};

#endif