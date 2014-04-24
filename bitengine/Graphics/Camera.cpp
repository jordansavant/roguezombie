#include "Camera.hpp"
#include "SFML/Graphics.hpp"
#include "../Math/VectorMath.hpp"
#include <cmath>

bit::Camera::Camera(sf::RenderWindow &window, float relativeX, float relativeY, float relativeWidth, float relativeHeight, int masterResolutionWidth, int masterResolutionHeight)
    : relativeRectangle(relativeX, relativeY, relativeWidth, relativeHeight),
      masterResolutionWidth(masterResolutionWidth),
      masterResolutionHeight(masterResolutionHeight),
      panSpeed(1),
      lockX(0),
      lockY(0),
      lockToleranceX(0),
      lockToleranceY(0),
      lockBufferAmount(0),
      lockBufferMax(1)
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

void bit::Camera::update(sf::Time &gameTime)
{
    sf::Vector2f destination = view.getCenter();

    // If we are moving the camera, ensure its in the bounds of our lock
    if (direction.x != 0 || direction.y != 0)
    {
        destination = view.getCenter() + bit::VectorMath::normalize(direction) * panSpeed;
        view.setCenter(destination);
        direction.x = 0;
        direction.y = 0;
    }

    sf::Vector2f newpos(destination);
    if(destination.x < lockX - lockToleranceX)
    {
        newpos.x = lockX - lockToleranceX;
    }
    else if(destination.x > lockX + lockToleranceX)
    {
        newpos.x = lockX + lockToleranceX;
    }
    if(destination.y < lockY - lockToleranceY)
    {
        newpos.y = lockY - lockToleranceY;
    }
    else if(destination.y > lockY + lockToleranceY)
    {
        newpos.y = lockY + lockToleranceY;
    }

    float xAmount = lockBufferAmount;
    float yAmount = lockBufferAmount;
    float max = lockBufferMax;
    float xf = std::abs(newpos.x - destination.x) / max;
    float yf = std::abs(newpos.y - destination.y) / max;
    xAmount *= xf;
    yAmount *= yf;

    bit::VectorMath::incrementTowards(destination.x, destination.y, newpos.x, newpos.y, xAmount, yAmount);

    view.setCenter(destination);
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

void bit::Camera::lock(float x, float y, float toleranceX, float toleranceY, float bufferSpeed, float bufferMax)
{
    lockX = x;
    lockY = y;
    lockToleranceX = toleranceX;
    lockToleranceY = toleranceY;
    lockBufferAmount = bufferSpeed;
    lockBufferMax = bufferMax;
}