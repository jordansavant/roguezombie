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

        Draggable(bit::InputManager* inputManager, bit::Element* element);

        bit::InputManager* inputManager;
        bit::Element* element;
        bool isDragging;
        bool centerOnMouse;
        sf::Time dragTime;
        float dragDistance;
        static float significantDragSeconds;
        static float signficantDragDistance;
        float dragOriginX, dragOriginY;
        float elementOriginX, elementOriginY;
        float centerOffsetX, centerOffsetY;
        std::function<void(Draggable*, bit::Element*)> onDragStart;
        std::function<bool(Draggable*, bit::Element*)> onDragStop;
        std::function<bool(Draggable*, bit::Element*)> checkDraggable;

        void update(sf::RenderWindow &window, sf::Time &gameTime);
        
        void cancel();

        bool isSignificantDrag();
    };
}

#endif