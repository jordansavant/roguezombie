#include "Checkbox.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Label.hpp"
#include "../Math/Math.hpp"

bit::CheckBox::CheckBox()
    : Label(), isChecked(false), checkedTexture(NULL), uncheckedTexture(NULL), textureOffsetX(0), textureOffsetY(0)
{
}

bit::CheckBox::CheckBox(sf::Texture &checkedTexture, sf::Texture &uncheckedTexture, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Label(relativeX, relativeY, width, height, anchorType), isChecked(false), checkedTexture(&checkedTexture), uncheckedTexture(&uncheckedTexture), textureOffsetX(0), textureOffsetY(0)
{
    checkedSprite.setTexture(checkedTexture);
    uncheckedSprite.setTexture(uncheckedTexture);
    currentSprite = isChecked ? &checkedSprite : &uncheckedSprite;
}

bit::CheckBox::CheckBox(sf::Texture &checkedTexture, sf::Texture &uncheckedTexture, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Label(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), isChecked(false), checkedTexture(&checkedTexture), uncheckedTexture(&uncheckedTexture), textureOffsetX(0), textureOffsetY(0)
{
    checkedSprite.setTexture(checkedTexture);
    uncheckedSprite.setTexture(uncheckedTexture);
    currentSprite = isChecked ? &checkedSprite : &uncheckedSprite;
}

void bit::CheckBox::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Set current sprite
    currentSprite = isChecked ? &checkedSprite : &uncheckedSprite;

    // Set target dimensions
    Label::updateTargets(window, gameTime);

    // Extend width to fit image
    targetWidth += currentSprite->getLocalBounds().width + textureOffsetX;
}

void bit::CheckBox::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Update real position of label
    Label::updateReals(window, gameTime);

    // Set Sprites details
    currentSprite->setColor(getColor());
    currentSprite->setScale(elementScale, elementScale);
    currentSprite->setPosition((int)(left + width - currentSprite->getLocalBounds().width * elementScale), (int)(top + textureOffsetY * elementScale));
}

void bit::CheckBox::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);

    if(opacity > 0)
    {
        window.draw(*this);

        window.draw(*currentSprite);
    }
}