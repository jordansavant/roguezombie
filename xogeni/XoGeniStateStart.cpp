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
    : bit::State(stack, _game), xoGeniGame(_game), levelGenerator(NULL), cellMap(NULL), seedCounter(0), difficultyCounter(0)
{
    createCamera(xoGeniGame, 0, 0, 1, 1);
    cameras[0]->panSpeed = 5;
    
    std::string fpsFontPath(resourcePath() + "xogeni/Agency.ttf");
    fps.load(fpsFontPath, 5, 3);
    fps.fpsText.setCharacterSize(11);
    fps.fpsText.setFillColor(sf::Color(100, 100, 100));

    levelGenerator = new LevelGenerator();
    levelRenderer = new XoLevelRenderer(this);

    // Seed
    seedCounter = 13726;// std::rand();
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
    if(ticks != 0 || xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::P))
    {
        //cameras[0]->changeZoom(ticks);
        levelRenderer->renderState++;
        levelRenderer->paint();
    }

    bool run = false;

    if(xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::Space))
    {
        run = true;
        if (xoGeniGame->inputManager->isButtonDown(sf::Keyboard::LShift))
        {
            seedCounter--;
            difficultyCounter = (difficultyCounter - 1) % CellMap::difficultyMax;
        }
        else
        {
            seedCounter++;
            difficultyCounter = (difficultyCounter + 1) % CellMap::difficultyMax;
        }
    }

    if(cellMap == NULL || run)
    {
        run = false;
        std::stringstream ss;
        ss << "SEED: " << seedCounter << std::endl;
        ss << "DIFF: " << difficultyCounter;
        bit::Output::Debug(ss.str());

        if(cellMap)
            delete cellMap;
        cellMap = levelGenerator->buildMap(seedCounter, difficultyCounter);
        levelRenderer->load(cellMap);

        // Center camera
        cameras[0]->setCenter(levelRenderer->getMapRenderSize().x / 2, levelRenderer->getMapRenderSize().y / 2);
    }

    if(xoGeniGame->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        // Special validation routine
        unsigned int testSeed = std::rand(); //seedCounter;
        unsigned int iterations = 10000;
        unsigned int difficultyLevel = 0;
        std::stringstream ssa;
        ssa << "TESTING " << iterations << " ITERATIONS FROM " << testSeed; 
        bit::Output::Debug(ssa.str());
        for(unsigned int i=0; i < iterations; i++)
        {
            if(i % 25 == 0)
            {
                std::stringstream ssb;
                ssb << "AT SEED " << testSeed;
                bit::Output::Debug(ssb.str());
            }

            CellMap* testMap = levelGenerator->buildMap(testSeed, difficultyLevel);

            if(!testMap->testValidity())
            {
                std::stringstream ssb;
                ssb << "SEED " << testSeed << " INVALID"; 
                bit::Output::Debug(ssb.str());
            }

            delete testMap;
            testSeed++;
        }
        bit::Output::Debug("TEST COMPLETE");
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