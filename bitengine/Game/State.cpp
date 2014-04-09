#include "State.hpp"
#include "StateStack.hpp"
#include "../Graphics/Camera.hpp"
#include "../Game/Game.hpp"
#include "../Game/VideoGame.hpp"
#include "SFML/Graphics.hpp"

bit::State::State(StateStack &stack, bit::Game* _game)
    : game(_game), stateStack(&stack), isPaused(false)
{
}

bit::State::~State()
{
    for(unsigned int i=0; i < cameras.size(); i++)
    {
        delete cameras[i];
    }
}

void bit::State::load()
{
}

bool bit::State::handleInput(sf::Time &gameTime)
{
    return true;
}

bool bit::State::update(sf::Time &gameTime)
{
    if(cameras.size() > 0)
    {
        for(unsigned int i = 0; i < cameras.size(); i++)
        {
            cameras[i]->update(gameTime);
        }

        // Ensure primary camera has modified the view for game elements
        Camera* camera = cameras[0];
        camera->renderWindow->setView(camera->view);
    }

    return true;
}

void bit::State::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(unsigned int i = 0; i < cameras.size(); i++)
    {
        Camera* camera = cameras[i];

        window.setView(camera->view);

        drawForCamera(window, gameTime, *camera);

        window.setView(window.getDefaultView());
    }

    window.setView(window.getDefaultView());
}

void bit::State::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
}

void bit::State::requestStackPush(unsigned int stateId)
{
    stateStack->pushState(stateId);
}

void bit::State::requestStackPop()
{
    stateStack->popState();
}

void bit::State::requestStateClear()
{
    stateStack->clearStates();
}

void bit::State::requestStateClearTo(unsigned int stateId)
{
    stateStack->clearStates();
    stateStack->pushState(stateId);
}

bit::Camera* bit::State::createCamera(bit::VideoGame* videoGame, float x, float y, float width, float height)
{
    Camera* camera = new Camera(*videoGame->renderWindow, x, y, width, height, bit::VideoGame::targetResolution.x, bit::VideoGame::targetResolution.y);
    cameras.push_back(camera);

    return camera;
}

void bit::State::handleWindowChange(sf::RenderWindow &window)
{
    for(unsigned int i = 0; i < cameras.size(); i++)
    {
        cameras[i]->handleWindowChange(window);
    }
}