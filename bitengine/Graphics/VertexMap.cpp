#include "VertexMap.h"
#include "SFML/Graphics.hpp"
#include "Vertex3.h"
#include "Vertex3Array.h"

bit::VertexMap::VertexMap()
    : texture(NULL), index(0), primitiveType(sf::PrimitiveType::Quads)
{
    vertexArray.setPrimitiveType(primitiveType);
}

void bit::VertexMap::load(sf::Texture* texture, sf::PrimitiveType primitiveType)
{
    this->texture = texture;
    this->primitiveType = primitiveType;
    vertexArray.setPrimitiveType(primitiveType);
}

unsigned int bit::VertexMap::requestVertexIndex()
{
    int size = 0;
    switch(primitiveType)
    {
        // Add more as needed
        case sf::PrimitiveType::Quads:
            size = 4;
            break;
    }

    // Expand its size
    vertexArray.resize(vertexArray.size() + size);

    // Return the index and increment
    unsigned int i = index;
    index += size;

    return i;
}

sf::PrimitiveType bit::VertexMap::getPrimitiveType()
{
    return primitiveType;
}