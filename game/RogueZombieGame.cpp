#include "RogueZombieGame.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Input.hpp"
#include "StartMenuState.hpp"
#include "GameplayState.hpp"
#include <iostream>
#include <sstream>

RogueZombieGame::RogueZombieGame()
	: Game("Rogue Zombie", 640, 480, false)
{
    Game::stateStack->pushState(startMenuState);
}

unsigned int RogueZombieGame::introState = 1;
unsigned int RogueZombieGame::startMenuState = 2;
unsigned int RogueZombieGame::hostState = 3;
unsigned int RogueZombieGame::clientState = 4;

void RogueZombieGame::update(sf::RenderWindow &window, sf::Time &gameTime)
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

void RogueZombieGame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
	Game::draw(window, gameTime);
}

void RogueZombieGame::registerStates()
{
    stateStack->registerState<StartMenuState>(startMenuState);
    stateStack->registerState<GameplayState>(hostState, true);
    stateStack->registerState<GameplayState>(clientState, false);
}