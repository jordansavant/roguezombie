#pragma once
#ifndef BIT_MOVEEFFECT_H
#define BIT_MOVEEFFECT_H

#include "SFML/Graphics.hpp"

#include "Effect.hpp"
#include "Easing.hpp"

namespace bit
{
    class Element;

    class MoveEffect : public Effect
    {
    public:

        MoveEffect(unsigned int milliseconds, float x, float y);

        MoveEffect(unsigned int milliseconds, Easing::Type easingType, float x, float y);

        MoveEffect(unsigned int milliseconds, Easing::Type easingType, float x, float y, std::function<void(Element*, Effect*)> onComplete);

        float originX;
        float originY;
        float distanceX;
        float distanceY;
        float previousX;
        float previousY;

        virtual void update(Element* element, sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif