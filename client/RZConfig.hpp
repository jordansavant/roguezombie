#pragma once
#ifndef RZ_RZCONFIG_H
#define RZ_RZCONFIG_H

#include "SFML/Graphics.hpp"

class RZConfig
{
public:

    static int zrenderRangeGameplay;
    static float zrenderTile;

    static float zrenderGameplayLowerbound;
    static float zrenderGameplayUpperbound;

    static float zrenderInterfaceLowerbound;
    static float zrenderInterfaceUpperbound;

    static sf::Color tileInteractColor;
    static sf::Color tileTargetedColor;
    static sf::Color tileMoveColor;
    static sf::Color tileTargetOutOfRange;
    static sf::Color tileTargetInRange;
    static sf::Color tileTargetInRangeRadius;

    static float getDrawDepthForGameplay(float yPosition);
};

#endif