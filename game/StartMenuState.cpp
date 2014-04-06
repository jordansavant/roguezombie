#include "StartMenuState.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"

StartMenuState::StartMenuState(bit::StateStack &stack, RogueZombieGame* _game)
    : bit::State(stack, _game), rogueZombieGame(_game), shape(200)
{
    rogueZombieGame->setVerticalSync(true);
    createCamera(rogueZombieGame, 0, 0, 1, 1);
}

bool StartMenuState::handleInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}

bool StartMenuState::update(sf::Time &gameTime)
{
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        requestStateClear();
        requestStackPush(RogueZombieGame::hostState);
    }
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Delete))
    {
        requestStateClear();
        requestStackPush(RogueZombieGame::clientState);
    }

    return true;
}

void StartMenuState::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);
}

void StartMenuState::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
	window.draw(shape);
}
