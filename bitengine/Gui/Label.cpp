#include "Label.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Math/Math.hpp"

bit::Label::Label()
    : Element(), fontSize(32), normalColor(sf::Color::White), focusedColor(sf::Color::White), paddingTop(0), paddingBottom(0), paddingLeft(0), paddingRight(0)
{
}

bit::Label::Label(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Element(relativeX, relativeY, width, height, anchorType), fontSize(32), normalColor(sf::Color::White), focusedColor(sf::Color::White), paddingTop(0), paddingBottom(0), paddingLeft(0), paddingRight(0)
{
}

bit::Label::Label(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Element(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), fontSize(32), normalColor(sf::Color::White), focusedColor(sf::Color::White), paddingTop(0), paddingBottom(0), paddingLeft(0), paddingRight(0)
{
}

void bit::Label::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    syncSizing();
}

void bit::Label::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    colorByFocus(isInfocus);
    // Scale the actual character size, not by transformable scale
    setCharacterSize(elementScale * fontSize);
    setPosition(left + (paddingLeft / 2) * elementScale, top + (paddingTop / 2) * elementScale);
}

void bit::Label::syncSizing()
{
    // Inverse our targets against our scale since we need to size
    // fonts by a real character size for aliasing issues in SFML
    targetWidth = (getLocalBounds().width / elementScale + paddingRight);
    targetHeight = (getLocalBounds().height / elementScale + paddingBottom);
}

void bit::Label::colorByFocus(bool focus)
{
    sf::Color color = focus ? focusedColor : normalColor;
    color.a = bit::Math::clamp(255 * opacity, 0, 255);
    setColor(color);
}

void bit::Label::setSfFont(sf::Font &font)
{
    setFont(font);
    syncSizing();
}

void bit::Label::setSfFontSize(unsigned int size)
{
    fontSize = size;
    syncSizing();
}

void bit::Label::setSfFontString(std::string &string)
{
    setString(string);
    syncSizing();
}

void bit::Label::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);

    if(opacity > 0)
        window.draw(*this);
}