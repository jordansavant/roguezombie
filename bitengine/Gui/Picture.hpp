#pragma once
#ifndef BIT_PICTURE_H
#define BIT_PICTURE_H

#include "SFML/Graphics.hpp"
#include "Element.hpp"

namespace bit
{
    class Picture : public Element, public sf::Sprite
    {
    public:

        Picture(float relativeX, float relativeY, float width, float height, AnchorType anchorType);

        Picture(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

        virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif