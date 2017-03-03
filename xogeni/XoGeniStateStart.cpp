#include "XoGeniStateStart.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "XoGeniGame.hpp"
#include "XoLevelRenderer.hpp"
#include "LevelGenerator/LevelGenerator.hpp"
#include "LevelGenerator/CellMap.hpp"
#include <sstream>

XoGeni::XoGeniStateStart::XoGeniStateStart(bit::StateStack &stack, XoGeniGame* _game)
    : bit::State(stack, _game), xoGeniGame(_game), levelGenerator(NULL), cellMap(NULL), seedCounter(0)
{
    createCamera(xoGeniGame, 0, 0, 1, 1);
    cameras[0]->panSpeed = 5;
    
    std::string fpsFontPath(resourcePath() + "xogeni/Agency.ttf");
    fps.load(fpsFontPath, 5, 3);
    fps.fpsText.setCharacterSize(11);
    fps.fpsText.setColor(sf::Color(100, 100, 100));

    levelGenerator = new LevelGenerator();
    levelRenderer = new XoLevelRenderer(this);

    // Seed
    seedCounter = std::rand();
}

XoGeni::XoGeniStateStart::~XoGeniStateStart()
{
    delete levelGenerator;
    delete levelRenderer;
    delete cellMap;
}

bool XoGeni::XoGeniStateStart::update(sf::Time &gameTime)
{
    bit::State::update(gameTime);

    fps.update(gameTime);

    if(xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStackPop();
    }
    
    // Move camera around
    if(xoGeniGame->inputManager->isButtonDown(sf::Keyboard::Up))
        cameras[0]->direction.y = -1;
    if(xoGeniGame->inputManager->isButtonDown(sf::Keyboard::Down))
        cameras[0]->direction.y = 1;
    if(xoGeniGame->inputManager->isButtonDown(sf::Keyboard::Left))
        cameras[0]->direction.x = -1;
    if(xoGeniGame->inputManager->isButtonDown(sf::Keyboard::Right))
        cameras[0]->direction.x = 1;
    
    if(xoGeniGame->inputManager->isButtonPressed(sf::Mouse::Middle))
        cameras[0]->resetZoom();

    int ticks = xoGeniGame->inputManager->getMouseWheelTicks();
    if(ticks != 0)
    {
        cameras[0]->changeZoom(ticks);
    }

    if(xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::Space))
    {
        std::stringstream ss;
        ss << "SEED: " << seedCounter;
        bit::Output::Debug(ss.str());

        cellMap = levelGenerator->generate(seedCounter, 90, 90);
        levelRenderer->load(cellMap);
        seedCounter++;
    }

    if(cellMap)
    {
        levelRenderer->update(gameTime);

        cameras[0]->lock(levelRenderer->getMapRenderSize().x / 2, levelRenderer->getMapRenderSize().y / 2,  levelRenderer->getMapRenderSize().x / 2, levelRenderer->getMapRenderSize().y / 2, 4.0, 100.0);
    }

    return true;
}


void XoGeni::XoGeniStateStart::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);

    fps.draw(window, gameTime);
}

void XoGeni::XoGeniStateStart::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
    if(cellMap)
    {
        window.draw(*levelRenderer);
    }
}