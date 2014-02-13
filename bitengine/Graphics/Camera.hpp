#pragma once
#ifndef BIT_CAMERA_H
#define BIT_CAMERA_H

#include "SFML/Graphics.hpp"
#include "../Math/VectorMath.hpp"

namespace bit
{
    class Camera
    {
    public:

        Camera(sf::RenderWindow &window, float relativeX, float relativeY, float relativeWidth, float relativeHeight, int masterResolutionWidth, int masterResolutionHeight);

        virtual ~Camera();

        enum LockMode
        {
            Point,
            Pan,
            OnBox
        };

        sf::RenderWindow* renderWindow;
        sf::View view;
        sf::FloatRect relativeRectangle;
        LockMode lockMode;
        sf::FloatRect* boxLock;
        sf::Vector2f lockPoint;
        sf::Vector2f panDirection;
        float panSpeed;
        float baseZoom;
        float currentZoom;
        int masterResolutionWidth;
        int masterResolutionHeight;
        sf::Vector2f tickOffset;
        sf::FloatRect rectangleOnScreen;
        sf::FloatRect rectangleInWorld;
        sf::Vector2f positionOnScreen;
        sf::Vector2f originInWorld;
        sf::Vector2f center;
        sf::Vector2f originReset;

        void resetZoom();

        virtual void changeZoom(int adjustBy);

        void handleWindowChange(sf::RenderWindow &window);

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void lockOnBox(sf::FloatRect* box);

        virtual void lockOnPoint(float x, float y);

        virtual void updateBoxLock(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updatePan(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void updatePointLock(sf::RenderWindow &window, sf::Time &gameTime);

        float translateXToCameraX(float x);

        float translateYToCameraY(float y);

        sf::Vector2f translateWorldPositionToCameraPosition(sf::Vector2f worldPosition);

        sf::Vector2f translateWorldPositionToScreenPosition(sf::Vector2f worldPosition);

    private:
        Camera();

        void setView();
    };
}

#endif