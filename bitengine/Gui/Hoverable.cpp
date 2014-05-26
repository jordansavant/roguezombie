#include "Hoverable.hpp"

bit::Hoverable::Hoverable(bit::InputManager* inputManager)
    : inputManager(inputManager), isHovering(false), onHoverEnter(NULL), onHoverLeave(NULL)
{
}

void bit::Hoverable::update(bit::Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::Vector2i mousePositionInScreen = sf::Mouse::getPosition(window);

    // If we are not hovering
    if(!isHovering)
    {
        // If the mouse is now hovering
        if(element->contains(mousePositionInScreen.x, mousePositionInScreen.y))
        {
            isHovering = true;
            if(onHoverEnter)
            {
                onHoverEnter(this, element);
            }
        }
    }
    // If we are hovering 
    else
    {
        // If the mouse is not in the element
        if(!element->contains(mousePositionInScreen.x, mousePositionInScreen.y))
        {
            isHovering = false;
            if(onHoverLeave)
            {
                onHoverLeave(this, element);
            }
        }
    }
}