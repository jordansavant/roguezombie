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

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

};

#endif