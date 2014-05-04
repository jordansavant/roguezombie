#pragma once
#ifndef BIT_DRAGGABLE_H
#define BIT_DRAGGABLE_H

#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Input/InputManager.hpp"
#include <functional>

namespace bit
{
    class Draggable
    {
    public:

        Draggable(bit::InputManager* inputManager);

        bit::InputManager* inputManager;
	    bool isDragging;
        float dragOriginX, dragOriginY;
        float elementOriginX, elementOriginY;
        std::function<void(Draggable*, bit::Element*)> onDragStart;
        std::function<bool(Draggable*, bit::Element*)> onDragStop;

        static bit::Element* draggingElement;
        static Draggable* draggingDraggable;
	
        void focusListener(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);
    };
}

#endif