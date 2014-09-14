#pragma once
#ifndef RZ_INVENTORYITEMLABEL_H
#define RZ_INVENTORYITEMLABEL_H

#include "Inventory.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"

class InventoryIcon;
class InventoryEquipmentSlot;
class InventoryPositionSlot;
class InventoryLootSlot;
class ActionBarSlot;

class InventoryItemLabel : public bit::Element
{
public:

    InventoryItemLabel(Hud* hud, ItemClient* item, float relativeX, float relativeY, AnchorType anchorType);

    virtual ~InventoryItemLabel();

    Hud* hud;
    Item::Schema itemSchema;
    InventoryIcon* icon;
    InventoryEquipmentSlot* currentEquipmentSlot;
    InventoryPositionSlot* currentPositionSlot;
    InventoryLootSlot* currentLootSlot;
    ActionBarSlot* currentActionSlot;

    void dropResult(bool result);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    bool dropOntoEquipmentSlot(InventoryEquipmentSlot* slot);

    bool dropOntoInventorySlot(InventoryPositionSlot* slot);

    bool dropOntoLootSlot(InventoryLootSlot* slot);

    bool dropOntoActionSlot(ActionBarSlot* slot);

};

#endif