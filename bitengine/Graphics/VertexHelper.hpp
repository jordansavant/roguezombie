#pragma once
#ifndef BIT_VERTEXHELPER_H
#define BIT_VERTEXHELPER_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class Vertex3;

    class VertexHelper
    {
    public:

        static void resetQuad(Vertex3* quad);

        static void positionQuad(Vertex3* quad, float x, float y, float z, float width, float height, float scale = 1);

        static void colorQuad(Vertex3* quad, sf::Color &color);

        static void rotateQuad(Vertex3* quad, float radians);

        static void rotateQuad(Vertex3* quad, float radians, float aroundX, float aroundY);

        static void flipQuad(Vertex3* quad, bool horizontal = true, bool vertical = false);
    };
}

#endif