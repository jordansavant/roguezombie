#pragma once
#ifndef RZ_LOOTMENU_H
#define RZ_LOOTMENU_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../items/ItemClient.hpp"

class Hud;

class LootMenu : public bit::Container
{
public:

    LootMenu(Hud* hud);

    Hud* hud;
    bit::Label* entries;
    ItemClient inventory;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void clear();

    void handleInventorySnapshot(bit::ServerPacket &packet, unsigned int tileId);
};

#endif