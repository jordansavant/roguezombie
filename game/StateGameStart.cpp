#include "StateGameStart.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"

StateGameStart::StateGameStart(bit::StateStack &stack, RogueZombieGame* _game)
    : bit::State(stack, _game), rogueZombieGame(_game), shape(200)
{
    createCamera(rogueZombieGame, 0, 0, 1, 1);
}

bool StateGameStart::handleInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}

bool StateGameStart::update(sf::Time &gameTime)
{
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        requestStateClear();
        requestStackPush(RogueZombieGame::stateGamePlayHost);
    }
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Delete))
    {
        requestStateClear();
        requestStackPush(RogueZombieGame::stateGamePlayClient);
    }

    return true;
}

void StateGameStart::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);
}

void StateGameStart::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
	window.draw(shape);
}
