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
    bit::Shadowcaster::computeFoV(x / level->tileWidth, y / level->tileHeight, level->tileColumns, level->tileRows, radius,
        std::bind(&Light::setVisible, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3),
        std::bind(&Light::isBlocked, this, std::placeholders::_1, std::placeholders::_2));
}

void Light::distributedUpdate(sf::Time &gameTime)
{
}

void Light::setVisible(int x, int y, float distance)
{
    Tile* t = level->getTileAtIndices(x, y);
    if(t && t->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
    {
        t->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;

        float bgA = t->schema.illumination;
        float bgR = (float)t->schema.rshade / 255;
        float bgG = (float)t->schema.gshade / 255;
        float bgB = (float)t->schema.bshade / 255;

        float fgA = (1 - distance) * brightness;
        float fgR = (float)color.r / 255;
        float fgG = (float)color.g / 255;
        float fgB = (float)color.b / 255;

        float rA = 0;
        float rR = 0;
        float rG = 0;
        float rB = 0;

        // mix colors additively
        if(bgA != 0 || fgA != 0)
        {
            rA = 1 - (1 - fgA) * (1 - bgA);
            rR = fgR * fgA / rA + bgR * bgA * (1 - fgA) / rA;
            rG = fgG * fgA / rA + bgG * bgA * (1 - fgA) / rA;
            rB = fgB * fgA / rA + bgB * bgA * (1 - fgA) / rA;
        }

        t->schema.illumination = rA;
        t->schema.rshade = rR * 255;
        t->schema.gshade = rG * 255;
        t->schema.bshade = rB * 255;

        if(t->body)
        {
            t->body->schema.illumination = rA;
            t->body->schema.rshade = rR * 255;
            t->body->schema.gshade = rG * 255;
            t->body->schema.bshade = rB * 255;
        }
        if(t->door)
        {
            t->door->schema.illumination = rA;
            t->door->schema.rshade = rR * 255;
            t->door->schema.gshade = rG * 255;
            t->door->schema.bshade = rB * 255;
        }

        return;
    }
}

bool Light::isBlocked(int x, int y)
{
    Tile* t = level->getTileAtIndices(x, y);

    return (t && t->body && t->body->blockFoV);
}