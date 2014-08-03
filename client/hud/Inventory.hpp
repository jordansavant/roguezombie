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

class Inventory : public HudMenu
{
public:

    Inventory(Hud* hud);

    bit::GameTimer refreshTimer;
    bit::Container* equipmentPanel;
    bit::Container* inventoryPanel;

    std::vector<InventoryEquipmentSlot*> equipmentSlotBoxes;
    //InventoryEquipmentSlot* headBox;
    //InventoryEquipmentSlot* chestBox;
    //InventoryEquipmentSlot* handBox;
    //InventoryEquipmentSlot* legBox;
    //InventoryEquipmentSlot* footBox;
    //InventoryEquipmentSlot* totemBox;
    //InventoryEquipmentSlot* primaryWeaponBox;
    //InventoryEquipmentSlot* secondaryWeaponBox;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void buildEquipment();

    void buildItemList(bool force = true);

    InventoryItemLabel* buildItem(ItemClient* i, float x, float y);

    virtual void hide();

    virtual void show();

};

#endif