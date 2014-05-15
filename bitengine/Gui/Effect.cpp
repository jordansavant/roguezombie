#include "Effect.hpp"
#include "SFML/Graphics.hpp"
#include "Easing.hpp"
#include <functional>

bit::Effect::Effect(unsigned int milliseconds)
    : duration(sf::milliseconds(milliseconds)), counter(sf::Time::Zero), isComplete(false),  hasUpdated(false), easingType(Easing::Type::Linear), onComplete(NULL)
{
}

bit::Effect::Effect(unsigned int milliseconds, Easing::Type easingType)
    : duration(sf::milliseconds(milliseconds)), counter(sf::Time::Zero), isComplete(false), hasUpdated(false), easingType(easingType), onComplete(NULL)
{
}

bit::Effect::Effect(unsigned int milliseconds, Easing::Type easingType, std::function<void(Element*, Effect*)> onComplete)
    : duration(sf::milliseconds(milliseconds)), counter(sf::Time::Zero), isComplete(false), hasUpdated(false), easingType(easingType), onComplete(onComplete)
{
}

bit::Effect::~Effect()
{
}

void bit::Effect::update(Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
    counter += gameTime;

    if(counter >= duration)
    {
        isComplete = true;

        finish(element, window, gameTime);

        if(onComplete)
        {
            onComplete(element, this);
        }
    }

    hasUpdated = true;
}

void bit::Effect::finish(Element* element, sf::RenderWindow &window, sf::Time &gameTime)
{
}

float bit::Effect::easing(float percentageComplete, float millisecondsSince, float startValue, float endValue, float totalDuration)
{
    bool negative = endValue < 0;
    float e = Easing::runByType(easingType, percentageComplete, millisecondsSince, startValue, endValue, totalDuration);
    return negative ? -e : e;
}