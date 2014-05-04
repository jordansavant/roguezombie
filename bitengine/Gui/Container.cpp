#include "Container.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Game/Game.hpp"
#include "../Math/Math.hpp"

bit::Container::Container()
    : Element(), childElements(), focusedChild(NULL), focusedChildIndex(-1)
{
}

bit::Container::Container(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Element(relativeX, relativeY, width, height, anchorType), childElements(), focusedChild(NULL), focusedChildIndex(-1)
{
}

bit::Container::Container(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Element(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), childElements(), focusedChild(NULL), focusedChildIndex(-1)
{
}

bit::Container::~Container()
{
    clearChildren();
}

void bit::Container::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::update(window, gameTime);

    for(auto it = childElements.begin(); it != childElements.end();)
    {
        // Update children
        (*it)->update(window, gameTime);

        // Remove any that are requested to be removed
        if ((*it)->removeFromParent)
        {
            delete (*it);
            it = childElements.erase(it);
        }
        else
        {
            ++it;
        }
    }

    updateInput(window, gameTime);
}

void bit::Container::updateInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(opacity > 0 && isInfocus || !parentElement)
    {
        // If I have input, all children cannot listen
        // This prevents newly focused children from reacting immediately
        if(!listenForInput(window, gameTime))
        {
            if(focusedChild != NULL)
            {
                focusedChild->listenForInput(window, gameTime);
            }
        }
    }
}

bool bit::Container::listenForInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(lambdaListenToInput)
        return lambdaListenToInput(this, &window, &gameTime);

    return false;
}

void bit::Container::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    debugRect.setPosition(left, top);
    debugRect.setFillColor(sf::Color(230, 0, 255, bit::Math::clamp(255 * opacity, 0, 80)));
    debugRect.setSize(sf::Vector2f(targetWidth, targetHeight));
    debugRect.setOutlineColor(sf::Color(255, 255, 255, bit::Math::clamp(255 * opacity, 0, 80)));
    debugRect.setScale(elementScale, elementScale);
    
    if(isInfocus)
    {
        debugRect.setOutlineThickness(2);
    }
    else
    {
        debugRect.setOutlineThickness(0);
    }
    
    window.draw(debugRect);

    for(unsigned int i = 0; i < childElements.size(); i++)
    {
        childElements[i]->draw(window, gameTime);
    }
}

unsigned int bit::Container::addChild(Element* child)
{
    childElements.push_back(child);
    child->parentElement = this;

    return childElements.size() - 1;
}

void bit::Container::clearFocusedChild()
{
    if(focusedChild)
    {
        focusedChild->isInfocus = false;
    }
    focusedChild = NULL;
    focusedChildIndex = -1;
}

void bit::Container::changeFocusedChild(unsigned int newChildIndex)
{
    if(focusedChild)
    {
        focusedChild->isInfocus = false;
    }
    focusedChildIndex = newChildIndex;
    focusedChild = childElements[focusedChildIndex];
    focusedChild->isInfocus = true;
}

void bit::Container::nextChild()
{
    if(childElements.size() > 0)
    {
        unsigned int count = 0;
        int next = focusedChildIndex;

        do
        {
            next = (next + 1) % childElements.size();

            // protect against forever
            if(count == childElements.size())
            {
                return;
            }
            count++;
        }
        while(!childElements[next]->canHaveFocus);

        changeFocusedChild(next);
    }
}

void bit::Container::previousChild()
{
    if(childElements.size() > 0)
    {
        unsigned int count = 0;
        int previous = focusedChildIndex;

        do
        {
            previous = (previous + childElements.size() - 1) % childElements.size();

            // protect against forever
            count++;
            if(count == childElements.size())
            {
                return;
            }
        }
        while(!childElements[previous]->canHaveFocus);

        changeFocusedChild(previous);
    }
}

void bit::Container::clearChildren()
{
    clearFocusedChild();
    for(unsigned int i = 0; i < childElements.size(); i++)
    {
        delete childElements[i];
    }
    childElements.clear();
}

void bit::Container::removeChild(unsigned int index)
{
    delete childElements[index];
    childElements.erase(childElements.begin() + index);
}