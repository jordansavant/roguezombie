#include "RZConfig.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Math.hpp"

int RZConfig::zrenderRangeGameplay = 2000;
float RZConfig::zrenderTile = 1;

float RZConfig::zrenderGameplayLowerbound = 0.0f;
float RZConfig::zrenderGameplayUpperbound = 0.8f;

float RZConfig::zrenderInterfaceLowerbound = 0.8f;
float RZConfig::zrenderInterfaceUpperbound = 1.0f;

sf::Color RZConfig::tileInteractColor(0, 255, 0);
sf::Color RZConfig::tileTargetedColor(0, 255, 0);
sf::Color RZConfig::tileMoveColor(255, 255, 255);
sf::Color RZConfig::tileTargetOutOfRange(255, 0, 0);
sf::Color RZConfig::tileTargetInRange(255, 0, 0);
sf::Color RZConfig::tileTargetInRangeRadius(255, 255, 0);

float RZConfig::getDrawDepthForGameplay(float yPosition)
{
    return 1 - bit::Math::calculateDrawDepthByRange(yPosition, zrenderRangeGameplay, zrenderGameplayLowerbound, zrenderGameplayUpperbound);
}
