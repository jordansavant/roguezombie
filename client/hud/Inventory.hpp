#pragma once
#ifndef RZ_INVENTORY_H
#define RZ_INVENTORY_H

#include "HudMenu.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"

class Hud;
class HudElement;
class InventoryItemLabel;
class InventoryEquipmentSlot;
class InventoryPositionSlot;

class Inventory : public HudMenu
{
public:

    Inventory(Hud* hud);

    int pendingInventoryRequests;
    bit::GameTimer refreshTimer;
    bit::Container* equipmentPanel;
    bit::Container* inventoryPanel;
    bit::Container* itemDataPanel;
    bit::Label* itemDataText;
    unsigned int naviconQuadIndex;
    bit::Sprite* naviconSprite;
    unsigned int siloQuadIndex;
    bit::Sprite* siloSprite;
    unsigned int maxInventoryCount;

    std::vector<InventoryEquipmentSlot*> equipmentSlotBoxes;
    std::vector<InventoryPositionSlot*> positionSlotBoxes;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void buildEquipment();

    void buildItemList(bool force = true);

    InventoryItemLabel* buildItem(ItemClient* i, float x, float y);

    virtual void hide();

    virtual void show();

    void showItemData(Item::Schema& itemSchema);

    void hideItemData();

    InventoryPositionSlot* getOpenSlot();
};

#endif