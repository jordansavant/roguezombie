#include "State.hpp"
#include "StateStack.hpp"
#include "../Graphics/Camera.hpp"
#include "../Game/Game.hpp"
#include "SFML/Graphics.hpp"

bit::State::State(StateStack &stack, bit::Game* _game)
    : game(_game), stateStack(&stack), isPaused(false)
{
}

void bit::State::load()
{
}

bool bit::State::handleInput(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}

bool bit::State::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}

void bit::State::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(unsigned int i = 0; i < cameras.size(); i++)
    {
        cameras[i]->update(window, gameTime);
    }

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

bit::Camera* bit::State::createCamera(sf::RenderWindow &window, float x, float y, float width, float height)
{
    Camera* camera = new Camera(window, x, y, width, height, bit::Game::targetResolution.x, bit::Game::targetResolution.y);
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