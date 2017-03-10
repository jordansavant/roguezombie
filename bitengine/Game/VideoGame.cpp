#include "VideoGame.hpp"
#include "SFML/Graphics.hpp"
#include "GameComponent.hpp"
#include "StateStack.hpp"
#include "../Input/InputManager.hpp"
#include "../Audio/SoundManager.hpp"
#include "../Audio/MusicManager.hpp"
#include "../Graphics/SpriteLoader.hpp"
#include "../Graphics/Vertex3Array.hpp"
#include <string>
#include <SFML/OpenGL.hpp>

bit::VideoGame::VideoGame(std::string gameTitle, int width, int height, bool fullscreen, unsigned int antialiasingLevel)
    : bit::Game()
{
    title = gameTitle;
    isInFocus = true;

    inputManager = new InputManager(this);
    soundManager = new SoundManager(this);
    musicManager = new MusicManager(this);
    spriteLoader = new SpriteLoader(this);

    gameComponents.push_back(inputManager);
    gameComponents.push_back(soundManager);
    gameComponents.push_back(musicManager);
    gameComponents.push_back(spriteLoader);

    bgColor = sf::Color::Black;

    settings.depthBits = 24;
    settings.stencilBits = 8;
    settings.antialiasingLevel = antialiasingLevel;
    settings.majorVersion = 3;
    settings.minorVersion = 0;

    changeResolution(width, height, false);
    isFullscreen = fullscreen;
    renderWindow = new sf::RenderWindow(sf::VideoMode(currentResolution.x, currentResolution.y, 32), title, (isFullscreen ? sf::Style::Fullscreen : sf::Style::Titlebar), settings);
    renderWindow->setVerticalSyncEnabled(verticalSync = false);
    configureOpenGL();
}

bit::VideoGame::~VideoGame()
{
    delete inputManager;
    delete soundManager;
    delete musicManager;
    delete spriteLoader;
    delete renderWindow;
}

bool bit::VideoGame::isInFocus = false;
bool bit::VideoGame::graphicsChange = false;
bool bit::VideoGame::isFullscreen = false;

sf::Vector2i bit::VideoGame::currentResolution;
sf::Vector2i bit::VideoGame::targetResolution = sf::Vector2i(1920, 1080);

float bit::VideoGame::currentResolutionRatio = 1;
float bit::VideoGame::currentResolutionRatioX = 1;
float bit::VideoGame::currentResolutionRatioY = 1;
float bit::VideoGame::targetFpsInterval = (1.0f / 60.0f); // seconds

void bit::VideoGame::run()
{
    registerStates();

    // Game loop
    running = true;
    while (running)
    {
        if(graphicsChange)
        {
            graphicsChange = false;
            renderWindow->create(sf::VideoMode(currentResolution.x, currentResolution.y, 32), title, (isFullscreen ? sf::Style::Fullscreen : sf::Style::Default), settings);
            configureOpenGL();
            setVerticalSync(verticalSync);
            stateStack->cascadeWindowEvent(*renderWindow);
        }

        // Freshen draw
        renderWindow->clear(bgColor);

        // UPDATE at fixed intervals
        float newTime = clock.getElapsedTime().asSeconds();
        float frameTime = newTime - actualTime;
        if(frameTime > 0.25f)
        {
            frameTime = 0.25f; // Avoid spiral of death
        }
        timer += frameTime;

        // Run update in dt sized chunks
        while(timer >= dt)
        {
            // Window listening
            sf::Event event;
            inputManager->preinspectEvents();
            while (renderWindow->pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    quit();

                if (event.type == sf::Event::GainedFocus)
                    isInFocus = true;

                if (event.type == sf::Event::LostFocus)
                    isInFocus = false;

                if (event.type == sf::Event::Resized)
                {
                    changeResolution(event.size.width, event.size.height);
                }

                // Special input events
                if (event.type == sf::Event::MouseWheelMoved)
                {
                    inputManager->inspectMouseWheelEvent(event);
                }
            }

            sf::Time gtu = sf::seconds(dt);
            update(gtu);
            timer -= dt;
        }

        // OpenGL Calls for clearing Depth
        float glR = (float)bgColor.r / (float)255;
        float glG = (float)bgColor.g / (float)255;
        float glB = (float)bgColor.b / (float)255;
        float glA = (float)bgColor.a / (float)255;
        glClearColor(glR, glG, glB, glA);
        glClearDepth(1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        sf::Time gtd = sf::seconds(newTime - actualTime);
        draw(*renderWindow, gtd);

        // end the current frame (internally swaps the front and back buffers)
        renderWindow->display();

        actualTime = newTime;
    }
}

void bit::VideoGame::update(sf::Time &gameTime)
{
    bit::Game::update(gameTime);
}

void bit::VideoGame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(unsigned int i=0; i<gameComponents.size(); i++)
    {
        gameComponents[i]->draw(window, gameTime);
    }
}

void bit::VideoGame::registerStates()
{
}

void bit::VideoGame::quit()
{
    running = false;
}

void bit::VideoGame::setVerticalSync(bool vSync)
{
    verticalSync = vSync;
    renderWindow->setVerticalSyncEnabled(vSync);
}

void bit::VideoGame::configureOpenGL()
{
    // Open GL Functions
    // - Enable and Disable depth in your specific draws where Vertex3's are used etc.
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    //glDepthRange(1.0f, 0.0f); // top = 1, bottom = 0  ** Commented out because this was causing major loss of precision, should be 0, 1
    glAlphaFunc(GL_GREATER, 0.0f);
}

void bit::VideoGame::changeResolution(int width, int height, bool change)
{
    graphicsChange = change;
    currentResolution.x = width;
    currentResolution.y = height;
    currentResolutionRatioX = (float)currentResolution.x / (float)targetResolution.x;
    currentResolutionRatioY = (float)currentResolution.y / (float)targetResolution.y;
    currentResolutionRatio = std::min(currentResolutionRatioX, currentResolutionRatioY);
}

void bit::VideoGame::changeFullscreen(bool fullscreen)
{
    graphicsChange = true;
    isFullscreen = fullscreen;
}

void bit::VideoGame::depthTestBegin()
{
    // Override SFML OpenGL Configurations
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
}

void bit::VideoGame::depthTestEnd()
{
    // Reset SFML OpenGL Configurations
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_ALPHA_TEST);
}