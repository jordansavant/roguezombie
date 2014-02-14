#include "Label.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Math/Math.hpp"

bit::Label::Label()
    : Element(), paddingTop(0), paddingBottom(0), paddingLeft(0), paddingRight(0)
{
}

bit::Label::Label(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Element(relativeX, relativeY, width, height, anchorType), paddingTop(0), paddingBottom(0), paddingLeft(0), paddingRight(0)
{
}

bit::Label::Label(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Element(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), paddingTop(0), paddingBottom(0), paddingLeft(0), paddingRight(0)
{
}

void bit::Label::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    targetWidth = getLocalBounds().width + paddingRight;
    targetHeight = getLocalBounds().height + paddingBottom;
}

void bit::Label::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    colorByFocus(isInfocus);
    setScale(elementScale, elementScale);
    setPosition(left + paddingLeft / 2, top + paddingTop / 2);
}

void bit::Label::colorByFocus(bool focus)
{
    sf::Color color = focus ? focusedColor : normalColor;
    color.a = bit::Math::clamp(255 * opacity, 0, 255);
    setColor(color);
}

void bit::Label::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);

    if(opacity > 0)
        window.draw(*this);
}