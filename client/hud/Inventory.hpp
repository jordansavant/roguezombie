#pragma once
#ifndef RZ_INVENTORY_H
#define RZ_INVENTORY_H

#include "HudMenu.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;
class ItemClient;
class InventoryItemLabel;
class InventoryEquipmentSlot;
class InventoryPositionSlot;

class Inventory : public HudMenu
{
public:

    Inventory(Hud* hud);

    bit::GameTimer refreshTimer;
    bit::Container* equipmentPanel;
    bit::Container* inventoryPanel;
    unsigned int itemCount;

    std::vector<InventoryEquipmentSlot*> equipmentSlotBoxes;
    std::vector<InventoryPositionSlot*> positionSlotBoxes;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void buildEquipment();

    void buildItemList(bool force = true);

    InventoryItemLabel* buildItem(ItemClient* i, float x, float y);

    virtual void hide();

    virtual void show();

};

#endif