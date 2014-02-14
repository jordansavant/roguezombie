#pragma once
#ifndef BIT_CHECKBOX_H
#define BIT_CHECKBOX_H

#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Label.hpp"
#include "../Math/Math.hpp"

namespace bit
{
    class CheckBox : public Label
    {
    public:

        CheckBox();

        CheckBox(sf::Texture &checkedTexture, sf::Texture &uncheckedTexture, float relativeX, float relativeY, float width, float height, AnchorType anchorType);

        CheckBox(sf::Texture &checkedTexture, sf::Texture &uncheckedTexture, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

        bool isChecked;
        sf::Texture* checkedTexture;
        sf::Texture* uncheckedTexture;
        sf::Sprite uncheckedSprite;
        sf::Sprite checkedSprite;
        sf::Sprite* currentSprite;
        float textureOffsetX;
        float textureOffsetY;

        virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif
