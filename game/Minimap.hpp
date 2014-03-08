#pragma once
#ifndef RZ_Minimap_H
#define RZ_Minimap_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "Tile.hpp"
#include <map>

class WorldClient;

class Minimap : public sf::Drawable, public sf::Transformable
{
public:

    Minimap();

    WorldClient* world;
    sf::Texture* texture;
    bit::Sprite* sprite;
    bit::VertexMap vertexMap;
    std::map<unsigned int, sf::Vector2i> points;

    void load(WorldClient* world, sf::Texture &texture);

    void addPoint(unsigned int tileId, int x, int y);

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif