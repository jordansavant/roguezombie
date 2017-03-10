#include "StateGameStart.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"
#include "menus/StartMainMenu.hpp"

StateGameStart::StateGameStart(bit::StateStack &stack, RogueZombieGame* _game)
    : bit::State(stack, _game), rogueZombieGame(_game)
{
    createCamera(rogueZombieGame, 0, 0, 1, 1);

    mainMenu = new StartMainMenu(rogueZombieGame, this);
    mainMenu->load();
}

StateGameStart::~StateGameStart()
{
    delete mainMenu;
}

bool StateGameStart::update(sf::Time &gameTime)
{
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStackPop();
    }
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        requestStackPush(RogueZombieGame::stateGamePlayHost);
    }
    // TODO: REMOVE
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Tilde))
    {
        rogueZombieGame->errorMessage = "Testing error message.";
        requestStackPush(RogueZombieGame::stateGameError);
    }

    mainMenu->update(*rogueZombieGame->renderWindow, gameTime);

    return true;
}

void StateGameStart::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);
    
    rogueZombieGame->bgColor = sf::Color(20, 20, 20);

    mainMenu->draw(window, gameTime);
}