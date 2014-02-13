#pragma once
#ifndef BIT_VERTEXMAP_H
#define BIT_VERTEXMAP_H

#include "SFML/Graphics.hpp"
#include "Vertex3Array.hpp"

namespace bit
{
    class VertexMap
    {
        public:
            VertexMap();

            Vertex3Array vertexArray;
            sf::Texture* texture;

            void load(sf::Texture* texture, sf::PrimitiveType primitiveType);

            unsigned int requestVertexIndex();

            sf::PrimitiveType getPrimitiveType();

    private:
            unsigned int index;
            sf::PrimitiveType primitiveType;
    };
}

#endif