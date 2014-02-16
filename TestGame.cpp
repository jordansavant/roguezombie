#include "TestGame.hpp"
#include "bitengine/Input/InputManager.hpp"
#include "bitengine/Game/StateStack.hpp"
#include "TitleState.hpp"
#include "MultiplayerState.hpp"
#include <iostream>
#include <sstream>
#include <SFML/OpenGL.hpp>

TestGame::TestGame()
	: Game("TEST GAME!!1!!", 1280, 720, false)
{
    Game::stateStack->pushState(titleState);
}

unsigned int TestGame::titleState = 1;
unsigned int TestGame::hostState = 2;
unsigned int TestGame::clientState = 3;

void TestGame::update(sf::RenderWindow &window, sf::Time &gameTime)
{
	Game::update(window, gameTime);

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
}

void TestGame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
	Game::draw(window, gameTime);
}

void TestGame::registerStates()
{
    stateStack->registerState<TitleState>(titleState);
    stateStack->registerState<MultiplayerState>(hostState, true);
    stateStack->registerState<MultiplayerState>(clientState, false);
}