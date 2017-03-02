#include "XoGeniStateStart.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "XoGeniGame.hpp"
#include "XoLevelRenderer.hpp"
#include "LevelGenerator/XoLevelGenerator.hpp"
#include "LevelGenerator/XoTileMap.hpp"

XoGeni::XoGeniStateStart::XoGeniStateStart(bit::StateStack &stack, XoGeniGame* _game)
    : bit::State(stack, _game), xoGeniGame(_game), levelGenerator(NULL), tileMap(NULL)
{
    createCamera(xoGeniGame, 0, 0, 1, 1);

    levelGenerator = new XoLevelGenerator();
    levelRenderer = new XoLevelRenderer(this);
}

XoGeni::XoGeniStateStart::~XoGeniStateStart()
{
    delete levelGenerator;
    delete levelRenderer;
    delete tileMap;
}

bool XoGeni::XoGeniStateStart::update(sf::Time &gameTime)
{
    if(xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStackPop();
    }

    if(xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::Space))
    {
        tileMap = levelGenerator->generate(std::rand(), 128, 128);
        levelRenderer->load(tileMap);
    }

    if(tileMap)
    {
        levelRenderer->update(gameTime);
    }

    return true;
}

void XoGeni::XoGeniStateStart::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);

    if(tileMap)
    {
        window.draw(*levelRenderer);
    }
}