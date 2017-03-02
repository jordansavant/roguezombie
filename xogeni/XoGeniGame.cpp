#include "XoGeniGame.hpp"
#include "../ResourcePath.h"
#include "../bitengine/Game.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Audio.hpp"
#include "XoGeniStateStart.hpp"
#include <iostream>
#include <sstream>

XoGeni::XoGeniGame::XoGeniGame()
    : VideoGame("XoGeni", 1280, 720, false), mouse(resourcePath() + "xogeni/mouse.png")
{
    Game::stateStack->pushState(stateStart);

    targetResolution.x = 1280;
    targetResolution.y = 720;
}

unsigned int XoGeni::XoGeniGame::stateStart = 1;

void XoGeni::XoGeniGame::update(sf::Time &gameTime)
{
    VideoGame::update(gameTime);

    if(isInFocus)
    {
        if(inputManager->isButtonPressed(sf::Keyboard::Numpad0))
            changeResolution(640, 480); // 4:3
        if(inputManager->isButtonPressed(sf::Keyboard::Numpad1))
            changeResolution(800, 600); // 4:3
        if(inputManager->isButtonPressed(sf::Keyboard::Numpad2))
            changeResolution(1024, 768); // 4:3
        if(inputManager->isButtonPressed(sf::Keyboard::Numpad3))
            changeResolution(1280, 720); // 16:9
        if(inputManager->isButtonPressed(sf::Keyboard::Numpad4))
            changeResolution(1440, 900); // 16:10
        if(inputManager->isButtonPressed(sf::Keyboard::Numpad5))
            changeResolution(1920, 1080); // 16:9
        if(inputManager->isButtonPressed(sf::Keyboard::Numpad6))
            changeResolution(2048, 1152); // 16:9
        if(inputManager->isButtonPressed(sf::Keyboard::Period))
            changeFullscreen(!isFullscreen);
        if(inputManager->isButtonPressed(sf::Keyboard::Comma))
            this->setVerticalSync(!this->verticalSync);
        if(inputManager->isButtonPressed(sf::Keyboard::Subtract))
            bit::Container::debugMode = !bit::Container::debugMode;
        if(inputManager->isButtonPressed(sf::Keyboard::Add))
            bit::Element::debugMode = !bit::Element::debugMode;
    }
}

void XoGeni::XoGeniGame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    VideoGame::draw(window, gameTime);

    mouse.draw(window, gameTime);
}

void XoGeni::XoGeniGame::registerStates()
{
    stateStack->registerState<XoGeniStateStart, XoGeniGame>(this, stateStart);
}
