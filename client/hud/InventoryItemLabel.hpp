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
class InventoryLootSlot;

class InventoryItemLabel : public bit::Label
{
public:

    InventoryItemLabel(Hud* hud, ItemClient* item, float relativeX, float relativeY, AnchorType anchorType);

    Hud* hud;
    Item::Schema itemSchema;
    InventoryEquipmentSlot* currentEquipmentSlot;
    InventoryPositionSlot* currentPositionSlot;
    InventoryLootSlot* currentLootSlot;

    bool dropOntoEquipmentSlot(InventoryEquipmentSlot* slot);

    bool dropOntoInventorySlot(InventoryPositionSlot* slot);

    bool dropOntoLootSlot(InventoryLootSlot* slot);

};

#endif