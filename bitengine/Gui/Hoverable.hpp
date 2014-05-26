#pragma once
#ifndef BIT_HOVERABLE_H
#define BIT_HOVERABLE_H

#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Input/InputManager.hpp"
#include <functional>

namespace bit
{
    class Hoverable
    {
    public:

        Hoverable(bit::InputManager* inputManager);

        bit::InputManager* inputManager;
	    bool isHovering;
        std::function<void(Hoverable*, bit::Element*)> onHoverEnter;
        std::function<void(Hoverable*, bit::Element*)> onHoverLeave;
	
        void update(bit::Element* element, sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif