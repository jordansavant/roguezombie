#pragma once
#ifndef BIT_MOUSE_H
#define BIT_MOUSE_H

#include <string>
#include "SFML/Graphics.hpp"

namespace bit
{
    class Mouse
    {
    public:
        Mouse(std::string textureName);

        sf::Texture texture;
        sf::Sprite sprite;
        sf::Color color;

        void draw(sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif