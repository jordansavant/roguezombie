#include "Element.hpp"
#include "SFML/Graphics.hpp"
#include <queue>
#include <list>
#include <functional>
#include "Effect.hpp"
#include "Draggable.hpp"
#include "Hoverable.hpp"
#include "../Game/VideoGame.hpp"
#include "../Math/Math.hpp"

bit::Element::Element()
    : sf::FloatRect(), parentElement(NULL), relativePosition(), anchorType(AnchorType::Center), scaleStyle(ScaleStyle::Smooth), opacity(1), elementScale(1), isInfocus(false), canHaveFocus(false), lambdaListenToInput(NULL), onAfterUpdate(NULL), onActivate(NULL), hasPositioned(false), removeFromParent(false), transitFromParent(false), draggable(NULL), hoverable(NULL)
{
}

bit::Element::Element(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : sf::FloatRect(0, 0, width, height), parentElement(NULL), relativePosition(relativeX, relativeY), anchorType(anchorType), scaleStyle(ScaleStyle::Smooth), opacity(1), elementScale(1), isInfocus(false), canHaveFocus(false), lambdaListenToInput(NULL), onAfterUpdate(NULL), onActivate(NULL), hasPositioned(false), removeFromParent(false), transitFromParent(false), draggable(NULL), hoverable(NULL)
{
    targetWidth = width;
    targetHeight = height;
}

bit::Element::Element(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : sf::FloatRect(0, 0, width, height), parentElement(NULL), relativePosition(relativeX, relativeY), anchorType(anchorType), scaleStyle(ScaleStyle::Smooth), opacity(1), elementScale(1), isInfocus(false), canHaveFocus(true), lambdaListenToInput(lambdaListenToInput), onAfterUpdate(NULL), onActivate(NULL), hasPositioned(false), removeFromParent(false), transitFromParent(false), draggable(NULL), hoverable(NULL)
{
    targetWidth = width;
    targetHeight = height;
}

bit::Element::~Element()
{
    clearEffects();

    delete draggable;
    delete hoverable;
}

void bit::Element::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
}

void bit::Element::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
}

void bit::Element::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    //debugRect.setPosition(left, top);
    //debugRect.setFillColor(sf::Color(255, 195, 0, bit::Math::clamp(255 * opacity * .2, 0, 255)));
    //debugRect.setSize(sf::Vector2f(width, height));
    //debugRect.setOutlineColor(sf::Color(255, 255, 255, bit::Math::clamp(255 * opacity * .2, 0, 255)));
    //
    //if(isInfocus)
    //{
    //    debugRect.setOutlineThickness(2);
    //}
    //else
    //{
    //    debugRect.setOutlineThickness(0);
    //}
    
    //window.draw(debugRect);
}

bit::Element* bit::Element::queueEffect(bit::Effect* effect)
{
    effectQueue.push_back(effect);

    return this;
}

bit::Element* bit::Element::immediateEffect(bit::Effect* effect)
{
    concurrentEffects.push_back(effect);

    return this;
}

bit::Element* bit::Element::clearEffects()
{
    for(unsigned int i = 0; i < effectQueue.size(); i++)
    {
        delete effectQueue[i];
    }
    effectQueue.clear();

    std::list<bit::Effect*>::iterator i = concurrentEffects.begin();
    while (i != concurrentEffects.end())
    {
        delete (*i);
        ++i;
    }
    concurrentEffects.clear();

    return this;
}

void bit::Element::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    elementScale = calculateViewRatio();

    // Allow elements to update their target dimensions
    updateTargets(window, gameTime);

    // Apply our scaling to our real dimensions
    width = elementScale * targetWidth;
    height = elementScale * targetHeight;

    // Apply effects
    updateEffects(window, gameTime);

    // Get our screen position
    sf::Vector2f offset = relativePosition * elementScale + calculateAnchor(window);

    // If we have a parent account for their position
    if(parentElement != NULL)
    {
        offset.x += parentElement->left;
        offset.y += parentElement->top;
    }

    // Assign real position on screen
    left = offset.x;
    top = offset.y;

    // Allow elements to update their items with real size data
    updateReals(window, gameTime);

    // Draggable
    if(draggable)
    {
        draggable->update(this, window, gameTime);
    }

    // Hoverable
    if(hoverable)
    {
        hoverable->update(this, window, gameTime);
    }

    // Custom updater
    if(onAfterUpdate)
    {
        onAfterUpdate(this, &window, &gameTime);
    }

    hasPositioned = true;
}

void bit::Element::updateInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(isInfocus)
    {
        listenForInput(window, gameTime);
    }
}

bool bit::Element::listenForInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    bool activated = false;
    if(lambdaListenToInput)
    {
        activated = lambdaListenToInput(this, &window, &gameTime);
    }

    if(activated && onActivate)
    {
        onActivate(this);
    }

    return false;
}

float bit::Element::calculateViewRatio()
{
    switch(scaleStyle)
    {
        case ScaleStyle::None:
            return 1;
        default:
        case ScaleStyle::Smooth:
            return std::min((float)VideoGame::currentResolution.x / (float)VideoGame::targetResolution.x, (float)VideoGame::currentResolution.y / (float)VideoGame::targetResolution.y);
        case ScaleStyle::PowerOfTwo:
            float smooth = std::min((float)VideoGame::currentResolution.x / (float)VideoGame::targetResolution.x, (float)VideoGame::currentResolution.y / (float)VideoGame::targetResolution.y);
            return bit::Math::roundPowerOf2(smooth);
    }
}

void bit::Element::updateEffects(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Queued effects
    if(effectQueue.size() > 0)
    {
        Effect* effect = effectQueue.front();

        if(effect)
        {
            effect->update(this, window, gameTime);
        }

        if(effect->isComplete)
        {
            effectQueue.pop_front();
            delete effect;
        }
    }

    // Concurrent effects
    std::list<Effect*>::iterator i = concurrentEffects.begin();
    while (i != concurrentEffects.end())
    {
        if(!(*i)->isComplete)
        {
            (*i)->update(this, window, gameTime);
        }

        if((*i)->isComplete)
        {
            delete (*i);
            concurrentEffects.erase(i++);
        }
        else
        {
            ++i;
        }
    }
}

sf::Vector2f bit::Element::calculateAnchor(sf::RenderWindow &window)
{
    sf::Vector2f anchorPositition;
    float parentWidth = 0;
    float parentHeight = 0;

    if(parentElement != NULL)
    {
        parentWidth = parentElement->width;
        parentHeight = parentElement->height;
    }
    else
    {
        parentWidth = window.getSize().x;
        parentHeight = window.getSize().y;
    }

    switch(anchorType)
    {
        default:
        case AnchorType::TopLeft:
            return sf::Vector2f(0, 0);
        case AnchorType::Top:
            return sf::Vector2f(0 - width / 2 + parentWidth / 2, 0);
        case AnchorType::TopRight:
            return sf::Vector2f(0 - width + parentWidth, 0);
        case AnchorType::Right:
            return sf::Vector2f(0 - width + parentWidth, 0 - height / 2 + parentHeight / 2);
        case AnchorType::BottomRight:
            return sf::Vector2f(0 - width + parentWidth, 0 - height + parentHeight);
        case AnchorType::Bottom:
            return sf::Vector2f(0 - width / 2 + parentWidth / 2, 0 - height + parentHeight);
        case AnchorType::BottomLeft:
            return sf::Vector2f(0, 0 - height + parentHeight);
        case AnchorType::Left:
            return sf::Vector2f(0, 0 - height / 2 + parentHeight / 2);
        case AnchorType::Center:
            return sf::Vector2f(0 - width / 2 + parentWidth / 2,  0 - height / 2 + parentHeight / 2);
    }
}

void bit::Element::makeDraggable(bit::InputManager* inputManager, std::function<void(Draggable*, bit::Element*)> onDragStart, std::function<bool(Draggable*, bit::Element*)> onDragStop)
{
    if(draggable == NULL)
    {
        draggable = new Draggable(inputManager);
        draggable->onDragStart = onDragStart;
        draggable->onDragStop = onDragStop;
    }
}

void bit::Element::makeHoverable(bit::InputManager* inputManager, std::function<void(Hoverable*, bit::Element*)> onHoverEnter, std::function<void(Hoverable*, bit::Element*)> onHoverLeave)
{
    if(hoverable == NULL)
    {
        hoverable = new Hoverable(inputManager);
        hoverable->onHoverEnter = onHoverEnter;
        hoverable->onHoverLeave = onHoverLeave;
    }
}