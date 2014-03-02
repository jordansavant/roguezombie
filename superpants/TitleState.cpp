#include "TitleState.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "TestGame.hpp"

TitleState::TitleState(bit::StateStack &stack, bit::Game* _game)
    : bit::State(stack, _game), shape(200)
{
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");
    zombiesprite.setTexture(zombieimage);

    createCamera(*game->renderWindow, 0, 0, 1, 1);
}

bool TitleState::handleInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}

bool TitleState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(game->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }
    else if(game->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        requestStateClear();
        requestStackPush(TestGame::hostState);
    }
    else if(game->inputManager->isButtonPressed(sf::Keyboard::Delete))
    {
        requestStateClear();
        requestStackPush(TestGame::clientState);
    }

    return true;
}

void TitleState::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);
}

void TitleState::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
	window.draw(shape);
    window.draw(zombiesprite);
}