#pragma once
#ifndef BIT_LABEL_H
#define BIT_LABEL_H

#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Math/Math.hpp"

namespace bit
{
    class Label : public Element, protected sf::Text
    {
    public:

        Label();

        Label(float relativeX, float relativeY, float width, float height, AnchorType anchorType);

        Label(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

        unsigned int fontSize;
        sf::Color normalColor;
        sf::Color focusedColor;
        int paddingTop;
        int paddingBottom;
        int paddingLeft;
        int paddingRight;

        virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

        void setSfFont(sf::Font &font);

        void setSfFontSize(unsigned int fontSize);

        void setSfFontString(std::string &string);

        void colorByFocus(bool focus);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif