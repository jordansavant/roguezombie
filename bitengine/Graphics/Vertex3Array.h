#pragma once
#ifndef BIT_VERTEX3ARRAY_H
#define BIT_VERTEX3ARRAY_H

#include "SFML/Graphics.hpp"
#include "SFML/OpenGl.hpp"
#include "Vertex3.h"

namespace bit
{
    class Vertex3Array : public sf::Drawable
    {
    public:

        Vertex3Array();

        Vertex3Array(sf::PrimitiveType type, unsigned int vertexCount);

        unsigned int Vertex3Array::getVertexCount() const;

        Vertex3& operator [](unsigned int index);

        const Vertex3& operator [](unsigned int index) const;

        void clear();

        unsigned int size();

        void resize(unsigned int vertexCount);

        void resize(unsigned int vertexCount, Vertex3 &val);

        void append(const Vertex3& vertex);

        void setPrimitiveType(sf::PrimitiveType type);

        sf::PrimitiveType getPrimitiveType() const;

        sf::FloatRect getBounds() const;

    private:

        std::vector<Vertex3> m_vertices;
        sf::PrimitiveType m_primitiveType;

        void draw(sf::RenderTarget& target, sf::RenderStates states) const;

        void applyCurrentView(sf::RenderTarget& target) const;

        sf::IntRect getViewport(sf::RenderTarget& target, const sf::View& view) const;
    };
}

#endif