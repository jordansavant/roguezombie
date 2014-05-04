#include "Draggable.hpp"

bit::Draggable::Draggable(bit::InputManager* inputManager)
    : inputManager(inputManager), isDragging(false), dragOriginX(0), dragOriginY(0), elementOriginX(0), elementOriginY(0), onDragStart(NULL), onDragStop(NULL)
{
}

bit::Element* bit::Draggable::draggingElement = NULL;
bit::Draggable* bit::Draggable::draggingDraggable = NULL;

void bit::Draggable::update(bit::Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::Vector2i mousePositionInScreen = sf::Mouse::getPosition(window);

    // If we are not dragging, recognize the drag
    if(!isDragging)
    {
        // If the mouse is hovering
        if(element->contains(mousePositionInScreen.x, mousePositionInScreen.y))
        {
            // If the mouse is pressed
            if(inputManager->isButtonPressed(sf::Mouse::Left))
            {
                isDragging = true;

                dragOriginX = mousePositionInScreen.x;
                dragOriginY = mousePositionInScreen.y;

                elementOriginX = element->relativePosition.x;
                elementOriginY = element->relativePosition.y;

                draggingElement = element;
                draggingDraggable = this;

                if(onDragStart)
                {
                    onDragStart(this, element);
                }
            }
        }
    }
    // If we are dragging 
    else
    {
        float currentX = mousePositionInScreen.x;
        float currentY = mousePositionInScreen.y;
            
        element->relativePosition.x = elementOriginX - (dragOriginX - currentX) / element->elementScale;
        element->relativePosition.y = elementOriginY - (dragOriginY - currentY) / element->elementScale;
            
        if(inputManager->isButtonReleased(sf::Mouse::Left))
        {
            isDragging = false;

            bool accept = true;
            if(onDragStop)
            {
                accept = onDragStop(this, element);
            }

            if(!accept)
            {
                element->relativePosition.x = elementOriginX;
                element->relativePosition.y = elementOriginY;
            }

            draggingElement = NULL;
            draggingDraggable = NULL;
        }
    }
}