#include "RZConfig.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Math.hpp"

int RZConfig::zrenderRangeGameplay = 2000;
float RZConfig::zrenderTile = 1;

float RZConfig::zrenderGameplayLowerbound = 0.0f;
float RZConfig::zrenderGameplayUpperbound = 0.8f;

float RZConfig::zrenderInterfaceLowerbound = 0.8f;
float RZConfig::zrenderInterfaceUpperbound = 1.0f;

sf::Vector2i RZConfig::_supportedResolutions[] = {
    sf::Vector2i(640, 480), // 4:3
    sf::Vector2i(800, 600), // 4:3
    sf::Vector2i(960, 540), // 16:9
    sf::Vector2i(1024, 768), // 4:3
    sf::Vector2i(1280, 720), // 16:9
    sf::Vector2i(1280, 800), // 16:10
    sf::Vector2i(1440, 900), // 16:10
    sf::Vector2i(1680, 1050), // 16:10
    sf::Vector2i(1920, 1080), // 16:9
    sf::Vector2i(1920, 1200), // 16:10
    sf::Vector2i(2048, 1152), // 16:9
    sf::Vector2i(2560, 1440), // 16:9
    sf::Vector2i(2560, 1600), // 16:10
    sf::Vector2i(4096, 2304), // 16:9
};
std::vector<sf::Vector2i> RZConfig::supportedResolutions(_supportedResolutions, _supportedResolutions + (sizeof _supportedResolutions / sizeof _supportedResolutions[0]));
unsigned int RZConfig::defaultResolutionIndex = 3;

sf::Color RZConfig::tileInteractColor(0, 255, 0);
sf::Color RZConfig::tileTargetedColor(0, 255, 0);
sf::Color RZConfig::tileMoveColor(255, 255, 255);
sf::Color RZConfig::tileTargetOutOfRange(255, 0, 0);
sf::Color RZConfig::tileTargetInRange(255, 0, 0);
sf::Color RZConfig::tileTargetInRangeRadius(255, 255, 0);

sf::Color RZConfig::fontGreen(0, 255, 0);
sf::Color RZConfig::fontRed(216, 30, 30);

float RZConfig::getDrawDepthForGameplay(float yPosition)
{
    return 1 - bit::Math::calculateDrawDepthByRange(yPosition, zrenderRangeGameplay, zrenderGameplayLowerbound, zrenderGameplayUpperbound);
}
