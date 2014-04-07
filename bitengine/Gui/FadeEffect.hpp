#pragma once
#ifndef BIT_FADEEFFECT_H
#define BIT_FADEEFFECT_H

#include "SFML/Graphics.hpp"

#include "../Game/Game.hpp"
#include "Effect.hpp"
#include "Element.hpp"
#include "Easing.hpp"

namespace bit
{
    class FadeEffect : public Effect
    {
    public:

        FadeEffect(unsigned int milliseconds, float targetOpacity);

        FadeEffect(unsigned int milliseconds, float targetOpacity, Easing::Type easingType);

        FadeEffect(unsigned int milliseconds, float targetOpacity, Easing::Type easingType, std::function<void(Element*, Effect*)> onComplete);

        float targetOpacity;
        float originOpacity;
        float previousOpacity;

        virtual void update(Element* element, sf::RenderWindow &window, sf::Time &gameTime);
    };
}

#endif