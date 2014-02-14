#pragma once
#ifndef BIT_CONTAINER_H
#define BIT_CONTAINER_H

#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Game/Game.hpp"

namespace bit
{
    class Container : public Element
    {
    public:

        Container();

        Container(float relativeX, float relativeY, float width, float height, AnchorType anchorType);

        Container(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

        virtual ~Container();

        std::vector<Element*> childElements;

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updatePosition(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateInput(sf::RenderWindow &window, sf::Time &gameTime);

        virtual bool listenForInput(sf::RenderWindow &window, sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual Element* addChild(Element* child);

        virtual void clearFocusedChild();

        virtual void changeFocusedChild(unsigned int newChildIndex);

        virtual void nextChild();

        virtual void previousChild();

    private:
        Element* focusedChild;
        int focusedChildIndex;
    };
}
#endif