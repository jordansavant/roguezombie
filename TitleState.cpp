#include "TitleState.hpp"
#include "bitengine/Game/Game.hpp"
#include "bitengine/Game/State.hpp"
#include "bitengine/Game/StateStack.hpp"
#include "bitengine/Graphics/Camera.hpp"
#include "ResourcePath.h"

TitleState::TitleState(bit::StateStack &stack)
    : bit::State(stack), shape(200)
{
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");
    zombiesprite.setTexture(zombieimage);

    createCamera(*bit::Game::instance->renderWindow, 0, 0, 1, 1);
}

bool TitleState::handleInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}


bool TitleState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
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