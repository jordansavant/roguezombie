#include "Container.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Game/Game.hpp"
#include "../Game/VideoGame.hpp"
#include "../Math/Math.hpp"

bool bit::Container::debugMode = false;

bit::Container::Container()
    : Element(), childElements(), managesOpacity(false), focusedChild(NULL), focusedChildIndex(-1), transferChild(NULL), fullscreen(false)
{
}

bit::Container::Container(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Element(relativeX, relativeY, width, height, anchorType), childElements(), managesOpacity(false), focusedChild(NULL), focusedChildIndex(-1), transferChild(NULL), fullscreen(false)
{
}

bit::Container::Container(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Element(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), childElements(), managesOpacity(false), focusedChild(NULL), focusedChildIndex(-1), transferChild(NULL), fullscreen(false)
{
}

bit::Container::~Container()
{
    clearChildren();

    if(transferChild)
        delete transferChild;
}

void bit::Container::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(fullscreen)
    {
        targetWidth = bit::VideoGame::targetResolution.x;
        targetHeight = bit::VideoGame::targetResolution.y;
    }
}

void bit::Container::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(fullscreen)
    {
        width = window.getSize().x;
        height = window.getSize().y;
    }
}


void bit::Container::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::update(window, gameTime);

    for(auto it = childElements.begin(); it != childElements.end();)
    {
        // Update children
        (*it)->update(window, gameTime);

        if(managesOpacity)
            (*it)->opacity = opacity;

        // Remove any that are requested to be removed
        if ((*it)->removeFromParent)
        {
            if((*it) == focusedChild)
                clearFocusedChild();
            delete (*it);
            it = childElements.erase(it);
        }
        // Something is taking over so remove it from the list but dont delete it
        else if((*it)->transitFromParent)
        {
            // Immediately add to the other parent
            // Hierarchy bug, see below
            if((*it)->transferToParent && (*it)->transferToParent != this)
            {
                bit::Container* otherParent = static_cast<bit::Container*>((*it)->transferToParent);
                otherParent->addChild((*it));
                otherParent->transferChild->transitFromParent = false;
                otherParent->transferChild->transferToParent = NULL;
                if(otherParent->transferChild->onTransmit)
                {
                    otherParent->transferChild->onTransmit(transferChild);
                    otherParent->transferChild->onTransmit = NULL;
                }
                otherParent->transferChild = NULL;
                (*it)->timeInParent = sf::seconds(0);
            }

            // Remove
            it = childElements.erase(it);
        }
        else
        {
            (*it)->timeInParent += gameTime;

            ++it;
        }
    }

    // If I have an incoming transfer child and it was not deleted from its old parent
    // Should only fire on swaps where moveChild is called on two containers
    // NOTE, THIS WILL EXPLODE IF I AM TRANSFERRING UP THE CONTAINER TREE, SOMEDAY FIX ALL OF THESE SHENANIGANS
    if(transferChild && transferChild->parentElement && transferChild->parentElement != this)
    {
        bit::Container* otherParent = static_cast<bit::Container*>(transferChild->parentElement);
        for(auto it = otherParent->childElements.begin(); it != otherParent->childElements.end();)
        {
            if((*it) == transferChild)
            {
                it = otherParent->childElements.erase(it);
                break;
            }
            else
            {
                ++it;
            }
        }
    }

    // If I have a transfer child, they should not be in the other container's list of children anymore
    if(transferChild)
    {
        addChild(transferChild);
        transferChild->transitFromParent = false;
        transferChild->transferToParent = NULL;
        if(transferChild->onTransmit)
        {
            transferChild->onTransmit(transferChild);
            transferChild->onTransmit = NULL;
        }
        transferChild = NULL;
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
    if(debugMode)
    {
        float o = std::max(opacity, .2f);
        debugRect.setPosition(left, top);
        debugRect.setFillColor(sf::Color(230, 0, 255, bit::Math::clamp(255 * o, 0, 80)));
        debugRect.setSize(sf::Vector2f(width, height));
        debugRect.setOutlineColor(sf::Color(255, 255, 255, bit::Math::clamp(255 * o, 0, 80)));
        if(isInfocus)
            debugRect.setOutlineThickness(2);
        else
            debugRect.setOutlineThickness(0);
        window.draw(debugRect);
    }

    for(unsigned int i = 0; i < childElements.size(); i++)
    {
        if(childElements[i]->hasPositioned)
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

void bit::Container::moveChild(Container* other, unsigned int index)
{
    Element* child = childElements[index];

    if(child == focusedChild)
    {
        clearFocusedChild();
    }

    child->transitFromParent = true;
    other->transferChild = child;
    child->transferToParent = other;
}

void bit::Container::moveChild(Container* other, Element* child)
{
    // find the child
    bool located = false;
    unsigned int location = 0;
    for(unsigned int i=0; i < childElements.size(); i++)
    {
        if(childElements[i] == child)
        {
            located = true;
            location = i;
        }
    }

    if(located)
    {
        moveChild(other, location);
    }
}

unsigned int bit::Container::findChildIndex(Element* child)
{
    for(unsigned int i=0; i < childElements.size(); i++)
    {
        if(childElements[i] == child)
        {
            return i;
        }
    }

    return 999999; // err
}