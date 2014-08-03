#pragma once
#ifndef RZ_MOVEMARKER_H
#define RZ_MOVEMARKER_H

#include "../bitengine/Graphics.hpp"

class LevelClient;

class MoveMarker
{
public:

    MoveMarker();

    LevelClient* level;
    bit::Sprite* sprite;
    unsigned int quadIndex;
    float renderX, renderY;

    void load(LevelClient* level);

    void renderAt(int worldX, int worldY, sf::Color &color);

    void hide();
};

#endif