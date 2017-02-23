#include "StateGameError.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "menus/ErrorMenu.hpp"
#include "RogueZombieGame.hpp"

StateGameError::StateGameError(bit::StateStack &stack, RogueZombieGame* _game)
    : bit::State(stack, _game), rogueZombieGame(_game)
{
    createCamera(rogueZombieGame, 0, 0, 1, 1);

    errorMenu = new ErrorMenu(rogueZombieGame, this);
    errorMenu->load();
}

StateGameError::~StateGameError()
{
    delete errorMenu;
}

bool StateGameError::update(sf::Time &gameTime)
{
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        requestStateClearTo(RogueZombieGame::stateGameStart);
    }

    errorMenu->update(*rogueZombieGame->renderWindow, gameTime);

    return true;
}

void StateGameError::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);

    errorMenu->draw(window, gameTime);
}