#include "Flare.hpp"

Flare::Flare()
    : Light(), startColor(255, 255, 255), endColor(0, 0, 0), startRadius(1), endRadius(0), startBrightness(1), endBrightness(0), timer(1), timerComplete(false)
{
}

void Flare::load(Level* level, float x, float y, float seconds, sf::Color &startColor, sf::Color &endColor, float startRadius, float endRadius, float startBrightness, float endBrightness)
{
    Light::load(level, x, y, startRadius, startColor, startBrightness);
    
    this->startColor = startColor;
    this->endColor = endColor;
    this->startRadius = startRadius;
    this->endRadius = endRadius;
    this->startBrightness = startBrightness;
    this->endBrightness = endBrightness;

    timer.setNewDuration(seconds);
}

void Flare::update(sf::Time &gameTime)
{
    if(!timerComplete)
    {
        // Check if flare has ended
        if(timer.update(gameTime))
        {
            removeFromLevel = true;
            timerComplete = true;
        }

        // Update light properties
        float ratio = timer.getCompletionRatio();

        Light::color.r = bit::Math::lerp(startColor.r, endColor.r, ratio);
        Light::color.g = bit::Math::lerp(startColor.g, endColor.g, ratio);
        Light::color.b = bit::Math::lerp(startColor.b, endColor.b, ratio);
        Light::brightness = bit::Math::lerp(startBrightness, endBrightness, ratio);
        Light::radius = bit::Math::lerp(startRadius, endRadius, ratio);

    }
    else
    {
        Light::color = sf::Color::Black;
    }

    Light::update(gameTime);
}
