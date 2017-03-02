#include "XoGeniStateStart.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "XoGeniGame.hpp"

XoGeni::XoGeniStateStart::XoGeniStateStart(bit::StateStack &stack, XoGeniGame* _game)
    : bit::State(stack, _game), xoGeniGame(_game)
{
    createCamera(xoGeniGame, 0, 0, 1, 1);
}

XoGeni::XoGeniStateStart::~XoGeniStateStart()
{
}

bool XoGeni::XoGeniStateStart::update(sf::Time &gameTime)
{
    if(xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStackPop();
    }

    return true;
}

void XoGeni::XoGeniStateStart::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);
}