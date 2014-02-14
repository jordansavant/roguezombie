#pragma once
#ifndef BIT_SLIDER_H
#define BIT_SLIDER_H

#include "SFML/Graphics.hpp"
#include "Label.hpp"

namespace bit
{
    class Slider : public Label
    {
    public:

        Slider(sf::Texture &handleTexture, sf::Texture &backgroundTexture, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<void(Slider*, float, sf::RenderWindow*, sf::Time*)> onSlideChange);

        std::function<void(Slider*, float, sf::RenderWindow*, sf::Time*)> onSlideChange;
        sf::Texture* handleTexture;
        sf::Texture* backgroundTexture;
        sf::Sprite handleSprite;
        sf::Sprite backgroundSprite;
        bool isChecked;
        float textureOffsetX;
        float textureOffsetY;
        float min;
        float current;
        float max;
        float sliderDistance;
        bool currentlySliding;

        virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif