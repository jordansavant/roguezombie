#pragma once
#ifndef BIT_EFFECT_H
#define BIT_EFFECT_H

#include "SFML/Graphics.hpp"
#include "Easing.hpp"
#include <functional>

namespace bit
{
    class Element;

    class Effect
    {
    public:

        Effect(unsigned int milliseconds);

        Effect(unsigned int milliseconds, Easing::Type easingType);

        Effect(unsigned int milliseconds, Easing::Type easingType, std::function<void(Element*, Effect*)> onComplete);

        virtual ~Effect();

        sf::Time duration;
        sf::Time counter;
        bool isComplete;
        bool hasUpdated;
        Easing::Type easingType;
        std::function<void(Element*, Effect*)> onComplete;

        virtual void update(Element* element, sf::RenderWindow &window, sf::Time &gameTime);

        virtual void finish(Element* element, sf::RenderWindow &window, sf::Time &gameTime);

        float easing(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration);
    };
}

#endif