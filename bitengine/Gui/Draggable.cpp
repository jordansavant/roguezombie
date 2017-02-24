#include "Draggable.hpp"

float bit::Draggable::significantDragSeconds = .2;
float bit::Draggable::signficantDragDistance = 3;

bit::Draggable::Draggable(bit::InputManager* inputManager, bit::Element* element)
    : inputManager(inputManager), element(element), isDragging(false), centerOnMouse(false), dragTime(), dragDistance(0),
      dragOriginX(0), dragOriginY(0), elementOriginX(0), elementOriginY(0), centerOffsetX(0), centerOffsetY(0),
      onDragStart(NULL), onDragStop(NULL), checkDraggable(NULL)
{
}

void bit::Draggable::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::Vector2i mousePositionInScreen = sf::Mouse::getPosition(window);

    // If we are not dragging, recognize the drag
    if(!isDragging)
    {
        // See if our element is currently allowing us to drag
        if(checkDraggable == NULL || checkDraggable(this, element))
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

                    if(centerOnMouse)
                    {
                        // If we are supposed to center the label on the mouse, record offset for that
                        centerOffsetX = mousePositionInScreen.x - (element->left + element->width / 2);
                        centerOffsetY = mousePositionInScreen.y - (element->top + element->height / 2);
                    }

                    elementOriginX = element->relativePosition.x;
                    elementOriginY = element->relativePosition.y;

                    if(onDragStart)
                    {
                        onDragStart(this, element);
                    }
                }
            }
        }
    }
    // If we are dragging 
    else
    {
        dragTime += gameTime;

        float currentX = mousePositionInScreen.x;
        float currentY = mousePositionInScreen.y;
        dragDistance = bit::VectorMath::distance(dragOriginX, dragOriginY, currentX, currentY);
        
        // Only visualize if we have been dragged a significant distance or time
        if(isSignificantDrag())
        {
            float dragDifferenceX = dragOriginX - currentX;
            float dragDifferenceY = dragOriginY - currentY;
            element->relativePosition.x = elementOriginX - (dragDifferenceX - centerOffsetX) / element->elementScale;
            element->relativePosition.y = elementOriginY - (dragDifferenceY - centerOffsetY) / element->elementScale;
        }
            
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

            // update tracking variables here so listeners can access the data anytime I am dragging
            dragTime = sf::seconds(0);
            dragDistance = 0;
        }
    }
}

void bit::Draggable::cancel()
{
    if(isDragging)
    {
        isDragging = false;
        element->relativePosition.x = elementOriginX;
        element->relativePosition.y = elementOriginY;
    }
}

bool bit::Draggable::isSignificantDrag()
{
    return (dragTime >= sf::seconds(significantDragSeconds) || dragDistance > signficantDragDistance);
}