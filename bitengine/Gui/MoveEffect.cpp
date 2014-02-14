#include "MoveEffect.hpp"
#include "SFML/Graphics.hpp"
#include "Effect.hpp"
#include "Element.hpp"
#include "Easing.hpp"

bit::MoveEffect::MoveEffect(unsigned int milliseconds, float x, float y)
    : Effect(milliseconds), distanceX(x), distanceY(y)
{
}

bit::MoveEffect::MoveEffect(unsigned int milliseconds, Easing::Type easingType, float x, float y)
    : Effect(milliseconds, easingType), distanceX(x), distanceY(y)
{
}

bit::MoveEffect::MoveEffect(unsigned int milliseconds, Easing::Type easingType, float x, float y, std::function<void(Element*, Effect*)> onComplete)
    : Effect(milliseconds, easingType, onComplete), distanceX(x), distanceY(y)
{
}

void bit::MoveEffect::update(Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    if(!hasUpdated)
    {
        previousX = originX = element->relativePosition.x;
        previousY = originY = element->relativePosition.y;
    }

    if(!isComplete)
    {
        float ratioTime = (float)counter.asMilliseconds() / (float)duration.asMilliseconds();
        float deltaTime = (float)gameTime.asMilliseconds() / (float)duration.asMilliseconds();

        float x = deltaTime * distanceX;
        float y = deltaTime * distanceY;

        float totalX = originX + easing(ratioTime, counter.asMilliseconds(), 0, 1, duration.asMilliseconds()) * distanceX;
        element->relativePosition.x += totalX - previousX;
        previousX = totalX;

        float totalY = originY + easing(ratioTime, counter.asMilliseconds(), 0, 1, duration.asMilliseconds()) * distanceY;
        element->relativePosition.y += totalY - previousY;
        previousY = totalY;
    }

    Effect::update(element, window, gameTime);
}