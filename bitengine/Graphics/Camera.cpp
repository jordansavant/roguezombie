#include "Camera.hpp"
#include "SFML/Graphics.hpp"
#include "../Math/VectorMath.hpp"

bit::Camera::Camera()
{
}

bit::Camera::Camera(sf::RenderWindow &window, float relativeX, float relativeY, float relativeWidth, float relativeHeight, int masterResolutionWidth, int masterResolutionHeight)
    : relativeRectangle(relativeX, relativeY, relativeWidth, relativeHeight), masterResolutionWidth(masterResolutionWidth), masterResolutionHeight(masterResolutionHeight)
{
    renderWindow = &window;
    lockMode = LockMode::Point;
    lockPoint.x = 0;
    lockPoint.y = 0;
    boxLock = NULL;
    panSpeed = 1;
    baseZoom = currentZoom = 100;

    setView();
}

bit::Camera::~Camera()
{
}

enum LockMode
{
    Point,
    Pan,
    OnBox
};

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
    switch (lockMode)
    {
        case LockMode::OnBox:
            updateBoxLock(window, gameTime);
            break;

        case LockMode::Pan:
            updatePan(window, gameTime);

            break;

        case LockMode::Point:
            updatePointLock(window, gameTime);
            break;
    }

    if (tickOffset.x != 0 || tickOffset.y != 0)
    {
        originReset = originInWorld;
        originInWorld += tickOffset;
    }
}

void bit::Camera::lockOnBox(sf::FloatRect* box)
{
    if (box == NULL)
    {
        boxLock = NULL;
        lockMode = LockMode::Point;
    }
    else
    {
        boxLock = box;
        lockMode = LockMode::OnBox;
    }
}

void bit::Camera::lockOnPoint(float x, float y)
{
    lockPoint.x = x - (rectangleInWorld.width / 2);
    lockPoint.y = y - (rectangleInWorld.height / 2);
    lockMode = LockMode::Point;
}

void bit::Camera::updateBoxLock(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(boxLock)
    {
        view.setCenter(sf::Vector2f(boxLock->left + boxLock->width / 2, boxLock->top + boxLock->height / 2));
    }
}

void bit::Camera::updatePan(sf::RenderWindow &window, sf::Time &gameTime)
{
    if (panDirection.x != 0 || panDirection.y != 0)
    {
        panDirection = bit::VectorMath::normalize(panDirection) * panSpeed;
        view.move(panDirection);
        panDirection.x = 0;
        panDirection.y = 0;
    }
}

void bit::Camera::updatePointLock(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(view.getCenter().x != lockPoint.x || view.getCenter().y != lockPoint.y)
    {
        view.setCenter(lockPoint);
    }
}

float bit::Camera::translateXToCameraX(float x)
{
    return x - originInWorld.x;
}

float bit::Camera::translateYToCameraY(float y)
{
    return y - originInWorld.y;
}

sf::Vector2f bit::Camera::translateWorldPositionToCameraPosition(sf::Vector2f worldPosition)
{
    return worldPosition - originInWorld;
}

sf::Vector2f bit::Camera::translateWorldPositionToScreenPosition(sf::Vector2f worldPosition)
{
    return translateWorldPositionToCameraPosition(worldPosition) + positionOnScreen;
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
