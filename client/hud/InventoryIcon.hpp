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
    bit::Sprite* sprite;

    void set(std::string &spritename);

    void position(int x, int y, int z, int width, int height);

    void reset();
};

#endif