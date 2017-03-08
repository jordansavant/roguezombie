#pragma once
#ifndef RZ_Minimap_H
#define RZ_Minimap_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../server/Tile.hpp"
#include "../../server/Structure.hpp"
#include <map>

class Hud;

class Minimap : public sf::Transformable
{
public:

    Minimap();

    struct Marker
    {
        Marker();

        enum Type
        {
            Ground,
            Wall,
            Door,
            Player,
            Downstairs,
            Upstairs,
        };
        unsigned int tileId;
        int x, y;
        unsigned int quadIndex;
        Minimap* minimap;
        bit::Sprite* sprite;
        Type type;

        void draw();
    };

    Hud* hud;
    bit::Sprite* markerSprite;
    bit::Sprite* playerSprite;
    bit::VertexMap vertexMap;
    std::map<unsigned int, Marker> points;
    Marker player;

    sf::Color groundColor;
    sf::Color wallColor;
    sf::Color doorColor;
    sf::Color playerColor;
    sf::Color stairsColor;
    unsigned char baseAlpha;

    void load(Hud* hud);

    void update(sf::Time& gameTime);

    void addPoint(unsigned int tileId, int x, int y, Tile::Type tileType, Structure::Type structureType);

    void setPlayerPosition(unsigned int tileId, int x, int y);

};

#endif