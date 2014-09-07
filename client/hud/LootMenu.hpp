#pragma once
#ifndef RZ_LOOTMENU_H
#define RZ_LOOTMENU_H

#include "HudMenu.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../items/ItemClient.hpp"

class Hud;
class InventoryItemLabel;
class InventoryLootSlot;

class LootMenu : public HudMenu
{
public:

    LootMenu(Hud* hud);

    ItemClient inventory;
    bool isActive;
    std::vector<InventoryLootSlot*> lootSlotBoxes;
    unsigned int tileId;
    unsigned int naviconQuadIndex;
    bit::Sprite* naviconSprite;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void activate();

    void deactivate();

    virtual void hide();

    virtual void show();

    void syncInventory();

    void handleInventorySnapshot(bit::ServerPacket &packet, unsigned int tileId);
};

#endif