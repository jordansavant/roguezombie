#include "Picture.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "../Math/Math.hpp"

bit::Picture::Picture(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Element(relativeX, relativeY, width, height, anchorType)
{
}

bit::Picture::Picture(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Element(relativeX, relativeY, width, height, anchorType, lambdaListenToInput)
{
}

void bit::Picture::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    targetWidth = getLocalBounds().width;
    targetHeight = getLocalBounds().height;
}

void bit::Picture::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::Color color = getColor();
    color.a = bit::Math::clamp(255 * opacity, 0, 255);
    setColor(color);
    setScale(elementScale, elementScale);
    setPosition(left, top);
}

void bit::Picture::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);

    if(opacity > 0)
        window.draw(*this);
}