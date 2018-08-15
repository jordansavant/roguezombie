#include "RogueZombieGame.hpp"
#include "../ResourcePath.h"
#include "../bitengine/Game.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Audio.hpp"
#include "StateGameStart.hpp"
#include "StateGamePlay.hpp"
#include "StateGameError.hpp"
#include <iostream>
#include <sstream>

RogueZombieGame::RogueZombieGame()
    : VideoGame("Rogue Zombie", 1280, 720, false, 0), errorMessage(""), mouse(resourcePath() + "mouse.png")
{
    Game::stateStack->pushState(stateGameStart);
    exploreMusic = musicManager->loadMusic(resourcePath() + "bit_song_heartbeat.ogg");
    combatMusic = musicManager->loadMusic(resourcePath() + "RockmenCombat.ogg");

    targetResolution.x = 1920;
    targetResolution.y = 1080;
}

unsigned int RogueZombieGame::stateGameIntroduction = 1;
unsigned int RogueZombieGame::stateGameStart = 2;
unsigned int RogueZombieGame::stateGamePlayHost = 3;
unsigned int RogueZombieGame::stateGamePlayClient = 4;
unsigned int RogueZombieGame::stateGamePlayLocal = 5;
unsigned int RogueZombieGame::stateGameError = 6;

void RogueZombieGame::update(sf::Time &gameTime)
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

void RogueZombieGame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    VideoGame::draw(window, gameTime);

    mouse.draw(window, gameTime);
}

void RogueZombieGame::registerStates()
{
    stateStack->registerState<StateGameStart, RogueZombieGame>(this, stateGameStart);
    stateStack->registerState<StateGamePlay, RogueZombieGame>(this, stateGamePlayHost, true, true, false);
    stateStack->registerState<StateGamePlay, RogueZombieGame>(this, stateGamePlayClient, true, false, false);
    stateStack->registerState<StateGamePlay, RogueZombieGame>(this, stateGamePlayLocal, true, true, true);
    stateStack->registerState<StateGameError, RogueZombieGame>(this, stateGameError);
}