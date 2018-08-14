#include "Slider.hpp"
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Label.hpp"
#include "../Game/Game.hpp"
#include "../Math/Math.hpp"
#include "../Input/InputManager.hpp"

bit::Slider::Slider(sf::Texture &handleTexture, sf::Texture &backgroundTexture, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<void(Slider* slider, float ratio)> onSlideChange)
    : Label(relativeX, relativeY, width, height, anchorType), onSlideChange(onSlideChange),
      handleTexture(&handleTexture), backgroundTexture(&backgroundTexture), textureOffsetX(0), textureOffsetY(0),
      min(0), current(0), max(1),
      currentlySliding(false), sliderDistance(0),
      lambdaOnSliderIsPressed(NULL),
      lambdaOnSliderIsReleased(NULL),
      setFromSystem(NULL)
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
    Label::updateReals(window, gameTime);

    // Set Default positions
    float backgroundX = left + width - (elementScale * backgroundSprite.getLocalBounds().width) + paddingRight + textureOffsetX * elementScale - handleSprite.getLocalBounds().width / 2 * elementScale;
    float backgroundY = top + textureOffsetY * elementScale;
    float handleMinX = backgroundX;
    float handleMaxX = backgroundX + (elementScale * backgroundSprite.getLocalBounds().width);
    sliderDistance = handleMaxX - handleMinX;
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);

    // Update state of sliding
    if(!currentlySliding)
    {
        // Listen to see if we start sliding
        if(lambdaOnSliderIsPressed && lambdaOnSliderIsPressed() && handleSprite.getGlobalBounds().contains(pixelPos.x, pixelPos.y))
        {
            currentlySliding = true;
        }
    }
    else
    {
        // Listen to see if we stop sliding
        if(currentlySliding && lambdaOnSliderIsReleased && lambdaOnSliderIsReleased())
        {
            currentlySliding = false;
        }
    }

    // Update element based on slider data
    if(currentlySliding)
    {
        float newSliderPosition = bit::Math::clamp((pixelPos.x - handleMinX) / sliderDistance, 0, 1);

        // If we have changed value, notify
        if(newSliderPosition != current)
        {
            onSlideChange(this, newSliderPosition);
            current = newSliderPosition;
        }
    }
    else
    {
        // reset our slider to system data
        if(setFromSystem)
        {
            current = setFromSystem();
        }
    }

    // Set handle to be at current value position
    float handleX = backgroundX + current * sliderDistance - (elementScale * handleSprite.getLocalBounds().width / 2);
    float handleY = top + textureOffsetY * elementScale;

    // Update drawables
    backgroundSprite.setColor(getFillColor());
    backgroundSprite.setScale(elementScale, elementScale);
    backgroundSprite.setPosition((int)backgroundX, (int)backgroundY);

    handleSprite.setColor(getFillColor());
    handleSprite.setScale(elementScale, elementScale);
    handleSprite.setPosition((int)handleX, (int)handleY);
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