#pragma once
#ifndef RZ_INVENTORY_H
#define RZ_INVENTORY_H

#include "HudMenu.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class Inventory : public HudMenu
{
public:

    Inventory(Hud* hud);

    bit::Label* entries;

    bit::Container* equipmentPanel;
    bit::Container* inventoryPanel;

    bit::Container* headBox;
    bit::Container* chestBox;
    bit::Container* handBox;
    bit::Container* legBox;
    bit::Container* footBox;
    bit::Container* totemBox;
    bit::Container* primaryWeaponBox;
    bit::Container* secondaryWeaponBox;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void buildItemList();

    virtual void hide();

    virtual void show();

};

#endif