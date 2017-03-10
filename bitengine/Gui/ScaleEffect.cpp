#include "ScaleEffect.hpp"
#include "../Game/Game.hpp"
#include "Effect.hpp"
#include "Element.hpp"
#include "Easing.hpp"
#include "../System.hpp"

bit::ScaleEffect::ScaleEffect(unsigned int milliseconds, float targetScale)
    : Effect(milliseconds), targetScale(targetScale), originScale(1)
{
}

bit::ScaleEffect::ScaleEffect(unsigned int milliseconds, float targetScale, Easing::Type easingType)
    : Effect(milliseconds, easingType), targetScale(targetScale), originScale(1)
{
}

bit::ScaleEffect::ScaleEffect(unsigned int milliseconds, float targetScale, Easing::Type easingType, std::function<void(Element*, Effect*)> onComplete)
    : Effect(milliseconds, easingType, onComplete), targetScale(targetScale), originScale(1)
{
}

void bit::ScaleEffect::update(Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    if(!hasUpdated)
    {
        previousScale = originScale = element->elementScale;
        previousWidth = originWidth = element->targetWidth;
        previousHeight = originHeight = element->targetHeight;
    }

    if(!isComplete)
    {
        float ratioTime = (float)counter.asMilliseconds() / (float)duration.asMilliseconds();
        float deltaTime = (float)gameTime.asMilliseconds() / (float)duration.asMilliseconds();

        float distance = (targetScale - originScale);
        float scaleThisInterval = distance * deltaTime;

        float e = easing(ratioTime, counter.asMilliseconds(), 0, 1, duration.asMilliseconds()) * distance;
        float ow = originWidth * e;
        float oh = originHeight * e;
        element->targetWidth = originWidth + ow;
        element->targetHeight = originHeight + oh;
        bit::Output::Debug(element->targetWidth);
        previousWidth = ow;
        previousHeight = oh;
    }

    Effect::update(element, window, gameTime);
}

void bit::ScaleEffect::finish(Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    element->targetWidth = originWidth * targetScale;
    element->targetHeight = originHeight * targetScale;
}
