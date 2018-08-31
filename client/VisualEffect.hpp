#pragma once
#ifndef RZ_VISUALEFFECT_H
#define RZ_VISUALEFFECT_H

#include "../bitengine/Game.hpp"

class LevelClient;
namespace bit {
    class Sprite;
}

class VisualEffect
{
public:

    enum Type
    {
        None,
        Poison,
        Heal
    };

    VisualEffect();

    VisualEffect(LevelClient* level);

    Type type;
    bit::GameTimer timer;
    bool isComplete;
    float worldX, worldY;
    LevelClient* level;
    bit::Sprite* sprite;
    unsigned int quadIndex;

    void load(Type type, float worldX, float worldY);

    void reset();

    void update(sf::Time &gameTime);

    static std::string getSpriteMoniker(Type type);
};

#endif