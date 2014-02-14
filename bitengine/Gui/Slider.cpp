#include "Slider.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Label.hpp"
#include "../Game/Game.hpp"
#include "../Math/Math.hpp"
#include "../Input/InputManager.hpp"

bit::Slider::Slider(sf::Texture &handleTexture, sf::Texture &backgroundTexture, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<void(Slider*, float, sf::RenderWindow*, sf::Time*)> onSlideChange)
    : Label(relativeX, relativeY, width, height, anchorType), onSlideChange(onSlideChange),
      handleTexture(&handleTexture), backgroundTexture(&backgroundTexture), textureOffsetX(0), textureOffsetY(0),
      min(0), current(0), max(1),
      currentlySliding(false), sliderDistance(0)
{
    handleSprite.setTexture(handleTexture);
    backgroundSprite.setTexture(backgroundTexture);
}

void bit::Slider::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Set target dimensions
    Label::updateTargets(window, gameTime);

    // Extend width to fit image
    targetWidth += backgroundSprite.getLocalBounds().width + handleSprite.getLocalBounds().width + textureOffsetX * elementScale;
}

void bit::Slider::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Update real position of label
    Label::updateReals(window, gameTime);

    // Set Background
    float backgroundX = left + width - (elementScale * backgroundSprite.getLocalBounds().width) + paddingRight + textureOffsetX * elementScale - handleSprite.getLocalBounds().width / 2 * elementScale;
    float backgroundY = top + textureOffsetY * elementScale;

    backgroundSprite.setColor(getColor());
    backgroundSprite.setScale(elementScale, elementScale);
    backgroundSprite.setPosition(backgroundX, backgroundY);

    float handleMinX = backgroundX;
    float handleMaxX = backgroundX + (elementScale * backgroundSprite.getLocalBounds().width);

    float lerpedCurrent = (current - min) / (max - min);
    sliderDistance = handleMaxX - handleMinX;

    float handleX = backgroundX + lerpedCurrent * sliderDistance - (elementScale * handleSprite.getLocalBounds().width / 2);
    float handleY = top + textureOffsetY * elementScale;

    // Listen for shitty input
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    if(bit::Game::inputManager->isButtonPressed(sf::Mouse::Left) && handleSprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
    {
        currentlySliding = true;
    }

    if(currentlySliding)
    {
        float val = (pixelPos.x - handleMinX) / sliderDistance;

        onSlideChange(this, val, &window, &gameTime);
    }
    else if(Game::inputManager->isButtonPressed(sf::Mouse::Left) && backgroundSprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
    {
        currentlySliding = true;
        float val = (pixelPos.x - handleMinX) / sliderDistance;

        onSlideChange(this, val, &window, &gameTime);
    }

    if(currentlySliding && Game::inputManager->isButtonReleased(sf::Mouse::Left))
    {
        currentlySliding = false;
    }

    if(currentlySliding)
    {
        handleX = bit::Math::clamp(pixelPos.x, handleMinX, handleMaxX) - (elementScale * handleSprite.getLocalBounds().width / 2);
    }

    handleSprite.setColor(getColor());
    handleSprite.setScale(elementScale, elementScale);
    handleSprite.setPosition(handleX, handleY);
}

void bit::Slider::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);

    if(opacity > 0)
    {
        window.draw(*this);

        window.draw(backgroundSprite);
        window.draw(handleSprite);
    }
}
