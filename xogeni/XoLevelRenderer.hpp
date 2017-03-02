#pragma once
#ifndef XG_XOLEVELRENDERER_H
#define XG_XOLEVELRENDERER_H

#include "../bitengine/Graphics.hpp"
#include <vector>

namespace XoGeni
{
    class XoGeniStateStart;
    class CellMap;

    class XoLevelRenderer : public sf::Drawable, public sf::Transformable
    {
    public:
        XoLevelRenderer(XoGeniStateStart* stateStart);
        
        ~XoLevelRenderer();

        XoGeniStateStart* stateStart;
        CellMap* cellMap;
        sf::Texture texture;
        bit::VertexMap vertexMap_tiles;
        std::vector<unsigned int>tileQuadIndexes;
        bit::Sprite* groundSprite;

        void load(CellMap* cellMap);

        void update(sf::Time &gameTime);

        sf::Vector2i getMapRenderSize();

    private:

        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    };
}
#endif