#include "Camera.hpp"
#include "SFML/Graphics.hpp"
#include "../Math/VectorMath.hpp"

bit::Camera::Camera(sf::RenderWindow &window, float relativeX, float relativeY, float relativeWidth, float relativeHeight, int masterResolutionWidth, int masterResolutionHeight)
    : relativeRectangle(relativeX, relativeY, relativeWidth, relativeHeight), masterResolutionWidth(masterResolutionWidth), masterResolutionHeight(masterResolutionHeight)
{
    renderWindow = &window;
    panSpeed = 1;
    baseZoom = currentZoom = 100;

    setView();
}

void bit::Camera::resetZoom()
{
    setView();
}

void bit::Camera::changeZoom(int adjustBy)
{
    currentZoom += adjustBy;
    view.zoom(100 / (baseZoom + adjustBy));
}

void bit::Camera::handleWindowChange(sf::RenderWindow &window)
{
    setView();
}

void bit::Camera::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    updatePan(window, gameTime);
}

void bit::Camera::updatePan(sf::RenderWindow &window, sf::Time &gameTime)
{
    if (direction.x != 0 || direction.y != 0)
    {
        direction = bit::VectorMath::normalize(direction) * panSpeed;
        view.move(direction);
        direction.x = 0;
        direction.y = 0;
    }
}

void bit::Camera::setView()
{
    float currentX = (float)renderWindow->getSize().x;
    float currentY = (float)renderWindow->getSize().y;
    float moveX = 0;
    float moveY = 0;

    view.reset(sf::FloatRect(0, 0, (int)currentX, (int)currentY));
    view.zoom(1);
    view.move(moveX, moveY);
    view.setViewport(relativeRectangle);
}