#pragma once
#ifndef RZ_FLARE_H
#define RZ_FLARE_H

#include "../Light.hpp"

class Flare : public Light
{
public:

    Flare();

    Level* level;
    sf::Color startColor, endColor;
    float startRadius, endRadius;
    float startBrightness, endBrightness;
    bit::GameTimer timer;
    bool timerComplete;

    virtual void load(Level* level, float x, float y, float seconds, sf::Color &startColor, sf::Color &endColor, float startRadius, float endRadius, float startBrightness, float endBrightness);

    virtual void update(sf::Time &gameTime);

};

#endif