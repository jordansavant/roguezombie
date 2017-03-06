#include "Light.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "Body.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Graphics.hpp"
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
    // Performance boost, do not process lights outside of player range
    if(level->isWithinRangeOfPlayer(x, y, 20 * level->tileWidth))
    {
        bit::Shadowcaster::computeFoV(x / level->tileWidth, y / level->tileHeight, level->tileColumns, level->tileRows, radius,
            std::bind(&Light::setVisible, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
            std::bind(&Light::isBlocked, this, std::placeholders::_1, std::placeholders::_2));
    }
}

void Light::distributedUpdate(sf::Time &gameTime)
{
}

void Light::setVisible(int x, int y, float distance)
{
    Tile* t = level->getTileAtIndices(x, y);
    if(t)
    {
        // Do not apply lighting twice for double inspected shadowcasting
        if(t->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
        {
            t->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;

            bit::ColorMixer colorMixer((float)t->schema.rshade / 255, (float)t->schema.gshade / 255, (float)t->schema.bshade / 255, t->schema.illumination);
            colorMixer.mixAdditive((float)color.r / 255, (float)color.g / 255, (float)color.b / 255, (1 - distance) * brightness);

            t->schema.illumination = colorMixer.a;
            t->schema.rshade = colorMixer.r * 255;
            t->schema.gshade = colorMixer.g * 255;
            t->schema.bshade = colorMixer.b * 255;

            if(t->body)
            {
                t->body->schema.illumination = colorMixer.a;
                t->body->schema.rshade = colorMixer.r * 255;
                t->body->schema.gshade = colorMixer.g * 255;
                t->body->schema.bshade = colorMixer.b * 255;
            }
            if(t->door)
            {
                t->door->schema.illumination = colorMixer.a;
                t->door->schema.rshade = colorMixer.r * 255;
                t->door->schema.gshade = colorMixer.g * 255;
                t->door->schema.bshade = colorMixer.b * 255;
            }
        }
    }
}

bool Light::isBlocked(int x, int y)
{
    Tile* t = level->getTileAtIndices(x, y);

    return (t && t->body && t->body->blockFoV);
}