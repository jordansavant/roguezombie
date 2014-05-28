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
        bool managesOpacity;
        bool fullscreen;

        virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updateInput(sf::RenderWindow &window, sf::Time &gameTime);

        virtual bool listenForInput(sf::RenderWindow &window, sf::Time &gameTime);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual unsigned int addChild(Element* child);

        virtual void clearFocusedChild();

        virtual void changeFocusedChild(unsigned int newChildIndex);

        virtual void nextChild();

        virtual void previousChild();

        virtual void clearChildren();

        virtual void removeChild(unsigned int childIndex);

        virtual void moveChild(Container* other, unsigned int childIndex);

        virtual void moveChild(Container* other, Element* child);

    private:
        Element* focusedChild;
        int focusedChildIndex;
        Element* transferChild;
    };
}
#endif