#include "RZConfig.hpp"
#include "../bitengine/Math.hpp"

int RZConfig::zrenderRangeGameplay = 2000;
float RZConfig::zrenderTile = 1;

float RZConfig::zrenderGameplayLowerbound = 0.0f;
float RZConfig::zrenderGameplayUpperbound = 0.8f;

float RZConfig::zrenderInterfaceLowerbound = 0.8f;
float RZConfig::zrenderInterfaceUpperbound = 1.0f;

float RZConfig::getDrawDepthForGameplay(float yPosition)
{
    return 1 - bit::Math::calculateDrawDepthByRange(yPosition, zrenderRangeGameplay, zrenderGameplayLowerbound, zrenderGameplayUpperbound);
}
