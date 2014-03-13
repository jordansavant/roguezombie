#pragma once
#ifndef BIT_VERTEXFONTMAP_H
#define BIT_VERTEXFONTMAP_H

#include "SFML/Graphics.hpp"
#include "Vertex3Array.hpp"
#include "VertexMap.hpp"

namespace bit
{
    class VertexFontMap
    {
        public:
            VertexFontMap();

            VertexMap vertexMap;
            sf::Font* font;
            sf::Texture texture;
            sf::Text text;
            std::string alpha;
            std::string symbol;
            std::string full;

            void load(sf::Font* font);

            void applyCharacter(Vertex3* vertex, std::string &character);
    };
}

#endif