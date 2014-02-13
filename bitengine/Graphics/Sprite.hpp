#pragma once
#ifndef BIT_SPRITE_H
#define BIT_SPRITE_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class Vertex3;

    class Sprite : public sf::IntRect
    {
        public:

            Sprite();

            Sprite(int left, int top, int width, int height);

            enum Flip
            {
                None,
                Horizontal,
                Verical,
                Both
            };
            Flip flip;

            void applyToQuad(Vertex3* quad);

            void applyToQuad(sf::Vertex* quad);
    };
}

#endif