#include "Light.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "Body.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Intelligence.hpp"
#include <functional>

Light::Light()
    : level(NULL)
{
}

void Light::load(Level* _level, float _x, float _y, float _radius, sf::Color _color, float _brightness)
{
    level = _level;
    x = _x;
    y = _y;
    radius = _radius;
    color = _color;
    brightness = _brightness;
}

void Light::update(sf::Time &gameTime)
{
    bit::Shadowcaster::computeFoV(x / level->tileWidth, y / level->tileHeight, level->tileRows, level->tileColumns, radius,
        std::bind(&Light::setVisible, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        std::bind(&Light::isBlocked, this, std::placeholders::_1, std::placeholders::_2));
}

void Light::setVisible(int x, int y, float distance)
{
    Tile* t = level->getTileAtIndices(x, y);
    if(t && t->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
    {
        t->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;

        // Calculate brightness
        float currentLight = t->deltaState.illumination;
        float addition = (1 - distance) * brightness;
        float combinedLight = currentLight + addition;
        float newLight = bit::Math::clamp(combinedLight, currentLight, 1.0f);
        t->deltaState.illumination = newLight;

        if(t->body)
        {
            t->body->deltaState.illumination = newLight;
        }

        // Calculate color shades based on brightness dominance and lighting
        // aF = aB / eB
        // eF = 1 - aB /eB + 1
        // if eB == .8 and aB == .2, the first lighting is brighter and should be more dominant,
        //   .2 / .8 = .25 correct
        //   1 - .25 + 1 = 1.75 correct
        // if eB == 1 and aB == 1
        //   1 / 1 = 1
        //   1 - 1 + 1 = 1
        // if eB == .9 and aB == 1
        //   aF = 1 / .9 = 1.11
        //   eF = 1 - 1.11 + 1 = .89
        // if eB == 0 and aB == .2
        //   aF = .2 / 0
        // if aB == .625 anf eB = .25
        //   aF = .625 / .25 = 2.5
        //   eF = 1 - 2.5 + 1 = -.5
        float existingBrightness = currentLight;
        float applyingBrightness = addition;
        float aF = 1;
        float eF = 1;
        if(existingBrightness == 0 && applyingBrightness == 0)
        {
            aF = 0;
            eF = 0;
        }
        else if(existingBrightness == 0)
        {
            aF = 1;
            eF = 0;
        }
        else if(applyingBrightness == 0)
        {
            aF = 0;
            eF = 1;
        }
        else
        {
            aF = applyingBrightness / existingBrightness;
            eF = 1 - aF + 1;
        }

        unsigned char r = bit::Math::clamp(((float)color.r * aF + (float)t->deltaState.rshade * eF) / (t->deltaState.rshade > 0 && color.r > 0 ? 2 : 1), 0,255);
        unsigned char g = bit::Math::clamp(((float)color.g * aF + (float)t->deltaState.gshade * eF) / (t->deltaState.gshade > 0 && color.g > 0 ? 2 : 1), 0, 255);
        unsigned char b = bit::Math::clamp(((float)color.b * aF + (float)t->deltaState.bshade * eF) / (t->deltaState.bshade > 0 && color.b > 0 ? 2 : 1), 0, 255);
        t->deltaState.rshade = r;
        t->deltaState.gshade = g;
        t->deltaState.bshade = b;
    }
}

bool Light::isBlocked(int x, int y)
{
    Tile* t = level->getTileAtIndices(x, y);

    return (t && t->body && t->body->fixedState.type == Body::Type::Structure);
}
