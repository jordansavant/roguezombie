#pragma once
#ifndef RZ_Minimap_H
#define RZ_Minimap_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../Tile.hpp"
#include <map>

class Hud;

class Minimap : public sf::Transformable
{
public:

    Minimap();

    Hud* hud;
    bit::Sprite* sprite;
    bit::VertexMap vertexMap;
    std::map<unsigned int, sf::Vector2i> points;

    void load(Hud* hud);

    void addPoint(unsigned int tileId, int x, int y);
};

#endif