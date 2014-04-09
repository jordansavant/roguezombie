#include "RogueZombieGame.hpp"
#include "../ResourcePath.h"
#include "../bitengine/Game.hpp"
#include "../bitengine/Input.hpp"
#include "StateGameStart.hpp"
#include "StateGamePlay.hpp"
#include "StateGameError.hpp"
#include <iostream>
#include <sstream>

RogueZombieGame::RogueZombieGame()
	: VideoGame("Rogue Zombie", 1280, 720, false), fps(), errorMessage("")
{
    std::string fpsFontPath(resourcePath() + "Agency.ttf");
    fps.load(fpsFontPath, 10, 10);

    Game::stateStack->pushState(stateGameStart);
}

unsigned int RogueZombieGame::stateGameIntroduction = 1;
unsigned int RogueZombieGame::stateGameStart = 2;
unsigned int RogueZombieGame::stateGamePlayHost = 3;
unsigned int RogueZombieGame::stateGamePlayClient = 4;
unsigned int RogueZombieGame::stateGameError = 5;

void RogueZombieGame::update(sf::Time &gameTime)
{
	VideoGame::update(gameTime);

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

    fps.update(gameTime);
}

void RogueZombieGame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
	VideoGame::draw(window, gameTime);

    fps.draw(window, gameTime);
}

void RogueZombieGame::registerStates()
{
    stateStack->registerState<StateGameStart, RogueZombieGame>(this, stateGameStart);
    stateStack->registerState<StateGamePlay, RogueZombieGame>(this, stateGamePlayHost, true, true);
    stateStack->registerState<StateGamePlay, RogueZombieGame>(this, stateGamePlayClient, true, false);
    stateStack->registerState<StateGameError, RogueZombieGame>(this, stateGameError);
}
