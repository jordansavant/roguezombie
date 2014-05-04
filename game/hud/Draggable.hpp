#pragma once
#ifndef RZ_DRAGGABLE_H
#define RZ_DRAGGABLE_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Input.hpp"
#include <functional>

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

#endif