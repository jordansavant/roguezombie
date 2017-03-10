#pragma once
#ifndef BIT_SCALEEFFECT_H
#define BIT_SCALEEFFECT_H

#include "SFML/Graphics.hpp"

#include "../Game/Game.hpp"
#include "Effect.hpp"
#include "Element.hpp"
#include "Easing.hpp"

namespace bit
{
    class ScaleEffect : public Effect
    {
    public:

        ScaleEffect(unsigned int milliseconds, float targetScale);

        ScaleEffect(unsigned int milliseconds, float targetScale, Easing::Type easingType);

        ScaleEffect(unsigned int milliseconds, float targetScale, Easing::Type easingType, std::function<void(Element*, Effect*)> onComplete);

        float targetScale;
        float originScale;
        float originWidth;
        float originHeight;
        float previousWidth;
        float previousHeight;
        float previousScale;

        virtual void update(Element* element, sf::RenderWindow &window, sf::Time &gameTime);

        virtual void finish(Element* element, sf::RenderWindow &window, sf::Time &gameTime);

    };
}

#endif