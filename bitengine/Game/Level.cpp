#include "Level.hpp"
#include "LevelComponent.hpp"
#include "../Graphics/Camera.hpp"
#include <vector>

bit::Level::Level(sf::Vector2i targetLevelResolution)
    : targetLevelResolution(targetLevelResolution)
{
    isInitialized = false;
    currentStep = LevelStateStep::Loading;
    allComponentsLoaded = false;
    allComponentsUnloaded = false;
}

bit::Level::~Level()
{
    // Items that create and assign Level Components should remove them
    // At least for this preliminary design

    // Cameras
    for(unsigned int i = 0; i < cameras.size(); i++)
    {
        delete cameras[i];
    }
}

void bit::Level::handleWindowChange(sf::RenderWindow &window)
{
    for(unsigned int i = 0; i < cameras.size(); i++)
    {
        cameras[i]->handleWindowChange(window);
    }
}

void bit::Level::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    if (!isInitialized)
    {
        initialize(window, gameTime);
        isInitialized = true;
    }

    bool someComponentNotLoaded = false;
    bool someComponentNotUnloaded = false;

    for(unsigned int i = 0; i < levelComponents.size(); i++)
    {
        LevelComponent* component = levelComponents[i];

        component->updateState(window, gameTime, allComponentsLoaded, allComponentsUnloaded);
        if (component->state == LevelComponent::LevelComponentState::Loading)
        {
            someComponentNotLoaded = true;
        }
        else if (component->state == LevelComponent::LevelComponentState::Unloading || component->state == LevelComponent::LevelComponentState::UnloadingForReload)
        {
            someComponentNotUnloaded = true;
        }
    }

    allComponentsLoaded = !someComponentNotLoaded;
    allComponentsUnloaded = !someComponentNotUnloaded;

    if (someComponentNotLoaded)
    {
        currentStep = LevelStateStep::Loading;
    }
    else if (someComponentNotUnloaded)
    {
        // empty on purpose
    }
    else if (currentStep == LevelStateStep::UnloadingForReload)
    {
        currentStep = LevelStateStep::Loading;
    }
    else if (currentStep == LevelStateStep::Unloading)
    {
        currentStep = LevelStateStep::Unloaded;
    }
    else if (currentStep != LevelStateStep::Unloaded)
    {
        currentStep = LevelStateStep::Running;
    }

    switch (currentStep)
    {
        case LevelStateStep::Loading:
            loadingUpdate(window, gameTime);
            break;
        case LevelStateStep::Running:

            // If a camera is defined, the first is the primary
            // Adjust the window for the camera so its screen coordinate mappings work
            if(cameras.size() > 0)
            {
                Camera* camera = cameras[0];
                window.setView(camera->view);
            }

            runningUpdate(window, gameTime);
            break;
        case LevelStateStep::Unloading:
            unloadingUpdate(window, gameTime);
            break;
        default:
            break;
    }
}

void bit::Level::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(allComponentsLoaded)
    {
        for(unsigned int i = 0; i < cameras.size(); i++)
        {
            cameras[i]->update(window, gameTime);
        }

        for(unsigned int i = 0; i < cameras.size(); i++)
        {
            Camera* camera = cameras[i];

            window.setView(camera->view);

            drawComponents(window, gameTime, *camera);

            window.setView(window.getDefaultView());
        }

        window.setView(window.getDefaultView());
    }

    switch(currentStep)
    {
        case LevelStateStep::Loading:
            loadingDraw(window, gameTime);
            break;
        case LevelStateStep::Running:
            runningDraw(window, gameTime);
            break;
        case LevelStateStep::Unloading:
            unloadingDraw(window, gameTime);
            break;
        default:
            break;
    }
}

void bit::Level::drawComponents(sf::RenderWindow &window, sf::Time &gameTime, Camera &camera)
{
    for(unsigned int i = 0; i < levelComponents.size(); i++)
    {
        levelComponents.at(i)->draw(window, gameTime, camera);
    }
}

float bit::Level::getPercentageLoaded()
{
    float totalPercentLoaded = 0;

    for(unsigned int i = 0; i < levelComponents.size(); i++)
    {
        totalPercentLoaded += levelComponents.at(i)->percentageLoaded;
    }

    return totalPercentLoaded / levelComponents.size();
}

void bit::Level::unloadComponents()
{
    currentStep = LevelStateStep::Unloading;

    for(unsigned int i = 0; i < levelComponents.size(); i++)
    {
        levelComponents.at(i)->state = LevelComponent::LevelComponentState::Unloading;
    }
}

void bit::Level::reloadComponents()
{
    currentStep = LevelStateStep::UnloadingForReload;

    for(unsigned int i = 0; i < levelComponents.size(); i++)
    {
        levelComponents.at(i)->state = LevelComponent::LevelComponentState::UnloadingForReload;
    }

    allComponentsUnloaded = false;
}

bit::Camera* bit::Level::createCamera(sf::RenderWindow &window, float x, float y, float width, float height)
{
    Camera* camera = new Camera(window, x, y, width, height, targetLevelResolution.x, targetLevelResolution.y);
    cameras.push_back(camera);

    return camera;
}

// Level Hooks
void bit::Level::initialize(sf::RenderWindow &window, sf::Time &gameTime)
{
}

void bit::Level::loadingUpdate(sf::RenderWindow &window, sf::Time &gameTime)
{
}
void bit::Level::runningUpdate(sf::RenderWindow &window, sf::Time &gameTime)
{
}
void bit::Level::unloadingUpdate(sf::RenderWindow &window, sf::Time &gameTime)
{
}

void bit::Level::loadingDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
}
void bit::Level::runningDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
}
void bit::Level::unloadingDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
}