#include "TestGame.hpp"
#include "bitengine/Input/InputManager.hpp"
#include "bitengine/Game/StateStack.hpp"
#include "TitleState.hpp"
#include <iostream>
#include <sstream>
#include <SFML/OpenGL.hpp>

TestGame::TestGame()
	: Game("TEST GAME!!1!!", 1280, 720, false)
{
    Game::stateStack->pushState(1);
}

void TestGame::update(sf::RenderWindow &window, sf::Time &gameTime)
{
	Game::update(window, gameTime);

    if(Game::inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        this->quit();
    }

    if(Game::inputManager->isButtonPressed(sf::Keyboard::Numpad0))
        Game::changeResolution(640, 480); // 4:3
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Numpad1))
        Game::changeResolution(800, 600); // 4:3
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Numpad2))
        Game::changeResolution(1024, 768); // 4:3
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Numpad3))
        Game::changeResolution(1280, 720); // 16:9
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Numpad4))
        Game::changeResolution(1440, 900); // 16:10
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Numpad5))
        Game::changeResolution(1920, 1080); // 16:9
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Numpad6))
        Game::changeResolution(2048, 1152); // 16:9
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Period))
        Game::changeFullscreen(!Game::isFullscreen);
    if(Game::inputManager->isButtonPressed(sf::Keyboard::Comma))
        this->setVerticalSync(!this->verticalSync);
}

void TestGame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
	Game::draw(window, gameTime);
}

void TestGame::registerStates()
{
    Game::stateStack->registerState<TitleState>(1);
}