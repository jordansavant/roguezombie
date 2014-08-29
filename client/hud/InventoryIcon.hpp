#pragma once
#ifndef RZ_InventoryIcon_H
#define RZ_InventoryIcon_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class InventoryIcon
{
public:
 
    InventoryIcon();
    
    InventoryIcon(Hud* hud);

    Hud* hud;
    unsigned int quadIndex;
    sf::Color color;
    bit::VertexMap* map;

    void set(std::string &spritename);

    void reset();
};

#endif