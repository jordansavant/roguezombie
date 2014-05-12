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

class LootMenu : public HudMenu
{
public:

    LootMenu(Hud* hud);

    bit::Label* entries;
    ItemClient inventory;
    bool isActive;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void activate();

    void deactivate();

    virtual void hide();

    virtual void show();

    void handleInventorySnapshot(bit::ServerPacket &packet, unsigned int tileId);
};

#endif