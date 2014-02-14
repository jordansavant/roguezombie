#include "LevelComponent.hpp"
#include "SFML/Graphics.hpp"
#include "../Graphics/Camera.hpp"

bit::LevelComponent::LevelComponent()
{
    isPausable = true;
    state = LevelComponentState::Loading;
    isLoaded = false;
    percentageLoaded = 0;
}

bit::LevelComponent::~LevelComponent()
{
}

bool bit::LevelComponent::load(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}

void bit::LevelComponent::update(sf::RenderWindow &window, sf::Time &gameTime)
{
}

void bit::LevelComponent::draw(sf::RenderWindow &window, sf::Time &gameTime, Camera &camera)
{
}

bool bit::LevelComponent::unload(sf::RenderWindow &window, sf::Time &gameTime)
{
    return true;
}

void bit::LevelComponent::updateState(sf::RenderWindow &window, sf::Time &gameTime, bool isGamePaused, bool allComponentsLoaded, bool allComponentsUnloaded)
{
    switch(state)
    {
        case LevelComponentState::Loading:
            if(load(window, gameTime))
            {
                state = LevelComponentState::WaitingForOtherComponentsToLoad;
                isLoaded = true;
                percentageLoaded = 1;
            }
            break;

        case LevelComponentState::WaitingForOtherComponentsToLoad:
            if (allComponentsLoaded)
            {
                state = LevelComponentState::Running;
            }
            break;

        case LevelComponentState::Running:
            if (!isGamePaused || !isPausable)
            {
                update(window, gameTime);
            }
            break;

        case LevelComponentState::Unloading:
            if (unload(window, gameTime))
            {
                state = LevelComponentState::Unloaded;
                isLoaded = false;
                percentageLoaded = 0;
            }
            break;

        case LevelComponentState::UnloadingForReload:
            if (unload(window, gameTime))
            {
                state = LevelComponentState::UnloadedWaitingForOtherComponentsToUnloadForReload;
                isLoaded = false;
                percentageLoaded = 0;
            }
            break;

        case LevelComponentState::UnloadedWaitingForOtherComponentsToUnloadForReload:
            if (allComponentsUnloaded)
            {
                state = LevelComponentState::Loading;
            }
            break;

        default:
        case LevelComponentState::Unloaded:
            // do nothing on purpose
            break;
    }
}