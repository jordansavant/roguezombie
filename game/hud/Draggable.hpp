#pragma once
#ifndef RZ_DRAGGABLE_H
#define RZ_DRAGGABLE_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Input.hpp"

class Draggable
{
public:

    Draggable(bit::InputManager* inputManager);

    bit::InputManager* inputManager;
	bool isDragging;
    float dragOriginX, dragOriginY;
    float elementOriginX, elementOriginY;
	
    void focusListener(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);
};

#endif