#include "FadeEffect.hpp"
#include "../Game/Game.hpp"
#include "Effect.hpp"
#include "Element.hpp"
#include "Easing.hpp"

bit::FadeEffect::FadeEffect(unsigned int milliseconds, float targetOpacity)
    : Effect(milliseconds), targetOpacity(targetOpacity), originOpacity(1)
{
}

bit::FadeEffect::FadeEffect(unsigned int milliseconds, float targetOpacity, Easing::Type easingType)
    : Effect(milliseconds, easingType), targetOpacity(targetOpacity), originOpacity(1)
{
}

bit::FadeEffect::FadeEffect(unsigned int milliseconds, float targetOpacity, Easing::Type easingType, std::function<void(Element*, Effect*)> onComplete)
    : Effect(milliseconds, easingType, onComplete), targetOpacity(targetOpacity), originOpacity(1)
{
}

void bit::FadeEffect::update(Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    if(!hasUpdated)
    {
        previousOpacity = originOpacity = element->opacity;
    }

    if(!isComplete)
    {
        float ratioTime = (float)counter.asMilliseconds() / (float)duration.asMilliseconds();
        float deltaTime = (float)gameTime.asMilliseconds() / (float)duration.asMilliseconds();

        float distance = (targetOpacity - originOpacity);
        float opacityThisInterval = distance * deltaTime;

        float o = originOpacity + easing(ratioTime, counter.asMilliseconds(), 0, 1, duration.asMilliseconds()) * distance;
        element->opacity += o - previousOpacity;
        previousOpacity = o;
    }

    Effect::update(element, window, gameTime);
}

void bit::FadeEffect::finish(Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    element->opacity = targetOpacity;
}
