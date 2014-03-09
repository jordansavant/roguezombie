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

        sf::RenderWindow* renderWindow;
        sf::View view;
        sf::FloatRect relativeRectangle;
        sf::Vector2f direction;

        float panSpeed;
        float baseZoom;
        float currentZoom;
        int masterResolutionWidth;
        int masterResolutionHeight;

        float lockX, lockY;
        float lockToleranceX, lockToleranceY;
        float lockBufferAmount, lockBufferMax;

        void resetZoom();

        virtual void changeZoom(int adjustBy);

        void handleWindowChange(sf::RenderWindow &window);

        virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void lock(float x, float y, float toleranceX, float toleranceY, float bufferSpeed = 2.0, float bufferMax = 30.0);

    private:

        void setView();
    };
}

#endif